#include "common.h"

#include <stdio.h>

extern const char CUR_SRC;


hash_t hash_msg(msg_t *msg) {
  // Djb2 hash implementation

  hash_t hash = 5381;
  uint8_t c;
  for (unsigned int i = 0; i < sizeof(msg_t) - sizeof(hash_t); i++) {
    c = ((char *)msg)[i];
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}


int verify_hash(msg_t *msg, hash_t hash) {
  return hash == hash_msg(msg);
}


void send_msg(msg_t *msg, scewl_id_t tgt_id, char cmd) {
  msg->src = CUR_SRC;
  msg->cmd = cmd;
  msg->hash = hash_msg(msg);
  fprintf(stderr, FMT_MSG("IN send_msg") "\n");
  
  
  if (scewl_send(tgt_id, sizeof(msg_t), (char *)msg) == SCEWL_ERR) {
    fprintf(stderr, FMT_MSG("Error sending message!") "\n");
  }
  fprintf(stderr, FMT_MSG("after scewl_send") "\n");
}


void send_faa_str(char *msg) {
  scewl_send(SCEWL_FAA_ID, strlen(msg), msg);
}


int recv_msg(msg_t* msg, scewl_id_t* src_id, scewl_id_t* tgt_id, size_t len, int blocking) {
  size_t size = scewl_recv((char *)msg, src_id, tgt_id, len, blocking);

  if (!blocking && size == SCEWL_NO_MSG) {
      return SCEWL_NO_MSG;
  }

  if (size < sizeof(msg_t)) {
    return 0;
  }

  // dispense recovery mode flag if checksum fails, RECOVERY_FLAG is defined, and the message isn't
  // from the FAA transceiver
  if (!verify_hash(msg, msg->hash)) {
#ifdef RECOVERY_FLAG
    if (*src_id != SCEWL_FAA_ID) {
      send_faa_str(STR(RECOVERY_FLAG));
    }
#endif
    return SCEWL_ERR;
  }

  return size;
}


int reg() {
  fprintf(stderr, FMT_MSG("Registering...") "\n");
  if (scewl_register() != SCEWL_OK) {
    fprintf(stderr, FMT_MSG("BAD REGISTRATION! Reregistering...") "\n");
    if (scewl_deregister() != SCEWL_OK) {
      fprintf(stderr, FMT_MSG("BAD DEREGISTRATION! CANNOT RECOVER") "\n");
      return 0;
    }
    if (scewl_register() != SCEWL_OK) {
      fprintf(stderr, FMT_MSG("BAD REGISTRATION! CANNOT RECOVER") "\n");
      return 0;
    }
  }
  fprintf(stderr, FMT_MSG("Done registering") "\n");
  return 1;
}


void dereg() {
  fprintf(stderr, FMT_MSG("Deregistering...") "\n");
  if (scewl_deregister() != SCEWL_OK) {
    fprintf(stderr, FMT_MSG("BAD DEREGISTRATION!") "\n");
  }
  fprintf(stderr, FMT_MSG("Done deregistering") "\n");
}


uint32_t prng(state_t *s) {
  // 32b MCG from https://arxiv.org/pdf/2001.05304.pdf
  s->prng_state = s->prng_state * 0xee5c155d;
  return s->prng_state;
}
