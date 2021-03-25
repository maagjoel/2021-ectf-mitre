/*
 * 2021 Collegiate eCTF
 * DZ SED
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


/**************************** defines ****************************
 *
 * Flags that need to be defined at compile:
 *  RECOVERY_FLAG
 */

#define DLEN 0x1000

const char CUR_SRC = DRP;



/**************************** handle message ****************************/

void handle_message(state_t *s, msg_t *msg, scewl_id_t src_id,
                    scewl_id_t tgt_id, int len) {
  scewl_id_t sender_id = src_id;
  int recvd_msgs = 0, retries = 0;
  (void)s;

  // ignore errors, no packet, FAA message, broadcast, non-UAV, or non-SYN
  if (len == SCEWL_ERR || len == SCEWL_NO_MSG || src_id == SCEWL_FAA_ID || SCEWL_BRDCST_ID
      || msg->src != UAV || msg->cmd != DZ_SYN_CMD) {
    return;
  }

  // get DZ response
  do {
    // resend request message every 16 received messages
    if ((recvd_msgs++ & 0xf) == 0) {
      if (retries++ == 3) {
        fprintf(stderr, FMT_MSG("DZ failed to recv package") "\n");
        return;
      }
      // no body to message
      send_msg(msg, sender_id, DZ_ACK_CMD);
    }

    // get a response
    len = recv_msg(msg, &src_id, &tgt_id, sizeof(msg_t), 1);
  } while (src_id != sender_id || msg->cmd != PACKAGE_CMD); // wait for sender response

  fprintf(stderr, FMT_MSG("Recvd package from %d") "\n", src_id);
}



/**************************** main ****************************/

int main(void) {
  scewl_id_t src_id, tgt_id;
  int len;
  char data[DLEN];
  state_t s;

  scewl_init();

  if (reg()) {
    // receive and handle messages forever
    while (1) {
      len = recv_msg((msg_t *)data, &src_id, &tgt_id, DLEN, 1);
      handle_message(&s, (msg_t *)data, src_id, tgt_id, len);
    }
  }

  // should never shut down, so never deregister
  fprintf(stderr, FMT_MSG("REGISTRATION FAILED! Shutting down...") "\n");
}

