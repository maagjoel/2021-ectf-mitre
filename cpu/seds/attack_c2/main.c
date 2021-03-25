/*
 * 2021 Collegiate eCTF
 * C2 SED
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 */

#include "common.h"

#include <stdio.h>

/**************************** defines ****************************/

#define DLEN 0x1000
#define ATCK_DZ_PROB 20

const char CUR_SRC = CMD;

// Flags that need to be defined at compile:
#ifndef PRNG_SEED
#warning C2 variables not not defined
#define PRNG_SEED 0xffff
#define ATCK_DZ_ID 0xffff
#define ATCK_DZ_X 0xffff
#define ATCK_DZ_Y 0xffff
#define TRST_DZ_ID 0xffff
#define PACKAGE_FLAG "ectf{package}"
#endif


/**************************** handle messages ****************************/

void handle_message(state_t *s, msg_t *msg, scewl_id_t src_id,
                    scewl_id_t tgt_id, int len) {
  (void)tgt_id;

  // ignore errors, no packet, FAA message, broadcast, non-UAV, or non-request
  if (len == SCEWL_ERR || len == SCEWL_NO_MSG || src_id == SCEWL_FAA_ID || SCEWL_BRDCST_ID
      || msg->src != UAV || msg->cmd != REQUEST_CMD) {
    return;
  }

  msg_t mission = {0};

  // Select a dropzone 
  if (prng(s) % 100 < ATCK_DZ_PROB) {
    mission.msg.mission.dz_id = ATCK_DZ_ID;
    mission.msg.mission.coord.x = ATCK_DZ_X;
    mission.msg.mission.coord.y = ATCK_DZ_Y;
  } else {
    mission.msg.mission.dz_id = TRST_DZ_ID;
    mission.msg.mission.coord.x = (prng(s) & 0x3fff) | 0x1000;
    mission.msg.mission.coord.y = (prng(s) & 0x3fff) | 0x1000;
  } 
  memcpy(mission.msg.mission.package, STR(PACKAGE_FLAG), PLEN);

  send_msg(&mission, src_id, MISSION_CMD);
}



/**************************** main ****************************/

int main(void) {
  state_t s;
  scewl_id_t src_id, tgt_id;
  int len;
  char data[DLEN];

  // seed prng
  s.prng_state = PRNG_SEED;

fprintf(stderr, FMT_MSG("C2 booted") "\n");
  scewl_init();

  // register
  if (reg()) {
    // handle packets forever
    while (1) {
      len = recv_msg((msg_t *)data, &src_id, &tgt_id, DLEN, 1);

      // ignore invalid packets
      if (len >= 0) {
        handle_message(&s, (msg_t *)data, src_id, tgt_id, len);
      }
    }
  }

  // should never shut down, so never deregister
fprintf(stderr, FMT_MSG("ERROR: C2 shutting down") "\n");
}
