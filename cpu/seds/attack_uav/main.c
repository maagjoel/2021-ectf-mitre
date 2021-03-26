/*
 * 2021 Collegiate eCTF
 * UAV SED
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 */

#include "common.h"
//#include "../scewl_bus_driver/scewl_bus.h"

#include <stdio.h>
#include <unistd.h>

#include <time.h>


/**************************** defines ****************************/

#define DLEN 0x1000
#define NBRDCST 10

// results in ~15m RTT on reference design
#define DIST_GRANULARITY 10
#define SLEEP_USEC 4000

const char CUR_SRC = UAV;

// Flags that need to be defined at compile:
#ifndef RECOVERY_FLAG
#warning "UAV variables not defined"
#define C2_ID 0xffff
#define ALT_CEIL 0xffff
#define ALT_FLOOR 0xffff
#define UAVID_FLAG "ectf{uavid}"
#define RECOVERY_FLAG "ectf{recovery}"
#define DROP_FLAG "ectf{drop}"
#define NO_FLY_ZONE_FLAG "ectf{nofly}"
#endif

void avoid_uav(state_t* s);
int drop_package(state_t *s, char* data);

  /**************************** send messages ****************************/

void broadcast_location(state_t *s) {
  msg_t sec_brdcst = {0};
  faa_location_msg_t faa_brdcst = {0};
   
  // send secure location broadcast
  sec_brdcst.msg.location.coord.x = s->x;
  sec_brdcst.msg.location.coord.y = s->y;
  sec_brdcst.msg.location.coord.z = s->z;
  strncpy(sec_brdcst.msg.location.uav_id, STR(UAVID_FLAG), FLEN);
  send_msg(&sec_brdcst, SCEWL_BRDCST_ID, LOCATION_CMD);

  // send FAA location broadcast
  faa_brdcst.id = SCEWL_ID;
  faa_brdcst.location.x = s->x;
  faa_brdcst.location.y = s->y;
  faa_brdcst.location.z = s->z;
  scewl_send(SCEWL_FAA_ID, sizeof(faa_brdcst), (char *)&faa_brdcst); 
}


/**************************** handle messages ****************************/

void handle_brdcst(state_t *s, msg_t *msg, scewl_id_t src_id, scewl_id_t tgt_id, int len) {
  (void)tgt_id;
  (void)len;
  // Send deconflict message if on same z-plane and not on ground
  if (msg->src == UAV && msg->cmd == LOCATION_CMD && s->z && msg->msg.location.coord.z == s->z) {
    msg_t deconflict_msg = {0};
    // no body to message
    send_msg(&deconflict_msg, src_id, DECONFLICT_CMD);
  }
}


void handle_c2(state_t *s, msg_t *msg, scewl_id_t src_id, scewl_id_t tgt_id, int len) {
  (void)tgt_id;
  (void)src_id;
  (void)len;
  if (msg->cmd == MISSION_CMD) {
    // Update mission
    s->tgt_dz_x = msg->msg.mission.coord.x;
    s->tgt_dz_y = msg->msg.mission.coord.y;
    s->tgt_dz_id = msg->msg.mission.dz_id;
    memcpy(s->package, msg->msg.mission.package, PLEN);
    s->registered = 1;
  }
}


void handle_uav(state_t *s, msg_t *msg, scewl_id_t src_id, scewl_id_t tgt_id, int len) {
  (void)tgt_id;
  (void)src_id;
  (void)len;

  // Check for avoid command if flying
  if (msg->cmd == DECONFLICT_CMD && s->z) {
    s->z += 5 + (prng(s) & 0xf);

    // Check if UAV has exceeded FAA ceiling
    if (s->z >= ALT_CEIL) {
      send_faa_str(STR(NO_FLY_ZONE_FLAG));
      s->direction = RETURNING;
    }

    broadcast_location(s);
  } 
}


void handle_drop(state_t *s, msg_t *msg, scewl_id_t src_id, scewl_id_t tgt_id, int len) {
  (void)tgt_id;
  (void)src_id;
  (void)len;

  // respond to DZ ACK with package
  if (msg->cmd == DZ_ACK_CMD) {
    msg_t pkg;

    memcpy(pkg.msg.package, s->package, PLEN);
    send_msg(&pkg, s->tgt_dz_id, PACKAGE_CMD);
  }
}


void handle_message(state_t *s, msg_t *msg, scewl_id_t src_id, scewl_id_t tgt_id, int len) {
  // ignore errors, no packet, or FAA messages
  if (len == SCEWL_ERR || len == SCEWL_NO_MSG || src_id == SCEWL_FAA_ID) {
    return;
  }

  // check for valid broadcast, otherwise ignore
  if (tgt_id == SCEWL_BRDCST_ID) {
    handle_brdcst(s, msg, src_id, tgt_id, len);
    return;
  }

  // if not a broadcast, target should be self
  if (tgt_id != SCEWL_ID) {
    return;
  }

  // Check for emergency drop package override
  // Correct message is 64B of "\x00\x01\x02...\xff\x00..."
  if (msg->cmd == EMERGENCY_CMD) {
    int match = 1;
    for (int i = 0; i < EMERGENCY_LEN; i++) {
      match &= msg->msg.emergency[i] == (i & 0xff);
    }

    // if match, send flag and return home
    if (match) {
      send_faa_str(STR(DROP_FLAG));
      s->direction = RETURNING;
      return;
    }
  }

  switch (msg->src) {
    case CMD:
      handle_c2(s, msg, src_id, tgt_id, len);
      break;
    case UAV:
      handle_uav(s, msg, src_id, tgt_id, len);
      break;
    case DRP:
      handle_drop(s, msg, src_id, tgt_id, len);
      break;
    default: // invalid source
      break;
  }
}



/**************************** UAV control ****************************/

int get_mission(state_t *s) {
  scewl_id_t src_id, tgt_id;
  msg_t request = {0};

  int len, recvd_msgs = 0, retries = 0;
  char data[DLEN];

  // C2 get mission request
  do {
    // resend request message every 16 received messages
    fprintf(stderr, FMT_MSG("IN DO") "\n");
    if ((recvd_msgs++ & 0xf) == 0) {
      if (retries++ == 3) {
        send_faa_str("UAV failed to connect to C2");
        return 0;
      }
    fprintf(stderr, FMT_MSG("BEFORE SEND MESSAGE") "\n");  
      // no body to message
      send_msg(&request, C2_ID, REQUEST_CMD);
    }

    fprintf(stderr, FMT_MSG("BEFORE RECIEVE MESSAGE") "\n");  
    // get a response
    len = recv_msg((msg_t *)data, &src_id, &tgt_id, DLEN, 1);
  } while (src_id != C2_ID); // ignore non-C2 messages

  fprintf(stderr, FMT_MSG("BEFORE HANDLE MESSAGE") "\n");  
  handle_message(s, (msg_t *)data, src_id, tgt_id, len);

  fprintf(stderr, FMT_MSG("BEFORE CORRECT RESPONSE") "\n"); 
  // check for correct response
  if (!s->registered || !s->tgt_dz_x || !s->tgt_dz_y) {
    send_faa_str("UAV failed to get mission from C2");
    return 0;
  }
  fprintf(stderr, FMT_MSG("BEFORE RETURN 1") "\n"); 

  return 1;
}

void move(state_t *s) {
  static int dist = 0, brdcsts = 0;

  if (!dist) {
    // move a major unit out
    if (s->direction == OUTGOING) {
      // don't move past target x/y
      if (s->x < s->tgt_dz_x) s->x++;
      if (s->y < s->tgt_dz_y) s->y++;

      // broadcast distance NBRDCST times going out
      int pct_traveled_weighted = ((NBRDCST - 1) * (s->x + s->y)) / (s->tgt_dz_x + s->tgt_dz_y);
      if (pct_traveled_weighted >= brdcsts) {
        broadcast_location(s);
        brdcsts++;
      }
    // move a major unit back
    } else {
      // don't move past origin
      if (s->x) s->x--;
      if (s->y) s->y--;

      // broadcast distance NBRDCST times coming back
      int pct_traveled_weighted = ((NBRDCST - 1) * (s->x + s->y)) / (s->tgt_dz_x + s->tgt_dz_y);
      if (((NBRDCST - 1) * 2) - pct_traveled_weighted >= brdcsts) {
        broadcast_location(s);
        brdcsts++;
      }
    }
  }

  // move a minor unit
  dist = (dist + 1) % DIST_GRANULARITY;
}


void deliver_package(state_t *s) {
  scewl_id_t src_id, tgt_id;
  msg_t msg;
  int len, recvd_msgs = 0, retries = 0;

  // drop down to DZ
  s->z = 0;

  // Send dropzone sync message 
  do {
    // resend sync message every 16 received messages
    if ((recvd_msgs++ & 0xf) == 0) {
      // give up after three retries
      if (retries++ == 3) {
        send_faa_str("UAV couldn't connect to the dropzone");
        break;
      }
      // no body to message
      send_msg(&msg, s->tgt_dz_id, DZ_SYN_CMD);
    }

    // get a response, sleeping up to 1m if no messages available
    for (int i = 0; i < 6; i++) {
      len = recv_msg(&msg, &src_id, &tgt_id, sizeof(msg_t), 0);
      if (len != SCEWL_NO_MSG) {
        break;
      }
      sleep(10);
    }
    handle_message(s, &msg, src_id, tgt_id, len);
  } while (src_id != s->tgt_dz_id); // ignore non-C2 messages

  // return home
  s->direction = RETURNING;
  s->z = ALT_FLOOR;
}


/**************************** main ****************************/

int main(void) {
  state_t s = {0};
  int len;
  scewl_id_t src_id, tgt_id;
  char data[DLEN];

  scewl_init();

  scewl_send(SCEWL_FAA_ID, 5 , "hello");
  scewl_send(SCEWL_FAA_ID, 8 , "helloguy");

  // seed prng with SCEWL_ID
  s.prng_state = SCEWL_ID;

  // only launch after registering and getting mission
  fprintf(stderr, FMT_MSG("Test Message") "\n");
  reg();
  dereg();
  fprintf(stderr, FMT_MSG("WE MADE IT") "\n");
  if (reg() && get_mission(&s)) {
    s.x = 0;
    s.y = 0;
    s.z = ALT_FLOOR;
    s.direction = OUTGOING;

    // fly until returned home
    while (s.direction == OUTGOING || s.x || s.y) {
      // move
      move(&s);

      // deliver package once at location
      if (s.direction == OUTGOING && s.x == s.tgt_dz_x && s.y == s.tgt_dz_y) {
        deliver_package(&s);
      }

      // handle message if available
      len = recv_msg((msg_t *)data, &src_id, &tgt_id, DLEN, 0);
      handle_message(&s, (msg_t *)data, src_id, tgt_id, len);

      // sleep a bit to travel distance
      usleep(SLEEP_USEC);
    }

    // deregister from SSS
    dereg();
  }
}

