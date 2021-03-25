#ifndef COMMON_H
#define COMMON_H

#include "scewl_bus_driver/scewl_bus.h"

#include <string.h>

#define DLEN 0x1000
#define FLEN 32
#define PLEN FLEN
#define EMERGENCY_LEN 64
#define STR_(X) #X
#define STR(X) STR_(X)
#define TAG STR(SCEWL_ID) ":"
#define FMT_MSG(M) TAG M ";"


/**************************** message types ****************************/

enum src_ty_t { CMD = 'C', UAV = 'U', DRP = 'D' };
enum cmd_ty_t { REQUEST_CMD = 'R', MISSION_CMD = 'M', LOCATION_CMD = 'L', DECONFLICT_CMD = 'D',
                DZ_SYN_CMD = 'S', DZ_ACK_CMD = 'A', PACKAGE_CMD = 'P', EMERGENCY_CMD = 'E' };
enum direction_t { OUTGOING = 1, RETURNING = -1 };

typedef uint32_t hash_t;

typedef struct state_t {
  uint16_t x;
  uint16_t y;
  uint16_t z;
  uint16_t tgt_dz_x;
  uint16_t tgt_dz_y;
  scewl_id_t tgt_dz_id;
  uint32_t prng_state;
  int registered;
  int direction;
  char package[PLEN];
} state_t;

typedef struct coord_t {
  uint16_t x;
  uint16_t y;
  uint16_t z;
} coord_t;

typedef struct mission_t {
  scewl_id_t dz_id;  
  coord_t coord;
  char package[PLEN];
} mission_t;

typedef struct scewl_location_t {
  coord_t coord;
  char uav_id[FLEN]; 
} scewl_location_t;

typedef struct msg_t {
  char src;
  char cmd;
  union {
    mission_t mission;
    scewl_location_t location;
    char package[PLEN];
    char emergency[EMERGENCY_LEN];
  } msg;
  hash_t hash;
} msg_t;

typedef struct faa_location_msg_t {
  scewl_id_t id;
  coord_t location;
} faa_location_msg_t;
	
/**************************** utility functions ****************************/

void send_msg(msg_t *msg, scewl_id_t tgt_id, char cmd);

int recv_msg(msg_t *buf, scewl_id_t *src_id, scewl_id_t *tgt_id, size_t len, int blocking); 

void send_faa_str(char *msg);

int reg();

void dereg();

uint32_t prng(state_t *s);

#endif // COMMON_H
