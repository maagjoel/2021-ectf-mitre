/*
 * 2021 Collegiate eCTF
 * SCEWL Bus Controller implementation
 * Ben Janis
 *
 * (c) 2021 The MITRE Corporation
 *
 * This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 */
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
 
// function to reverse buffer[i..j]
char* reverse(char *buffer, int i, int j)
{
    while (i < j)
        swap(&buffer[i++], &buffer[j--]);
 
    return buffer;
}
 
// Iterative function to implement itoa() function in C
char* itoa(unsigned long value, char* buffer, int base)
{
    // invalid input
    if (base < 2 || base > 32)
        return buffer;
 
    // consider absolute value of number
    unsigned long n = value;
 
    int i = 0;
    while (n)
    {
        int r = n % base;
 
        if (r >= 10) 
            buffer[i++] = 65 + (r - 10);
        else
            buffer[i++] = 48 + r;
 
        n = n / base;
    }
 
    // if number is 0
    if (i == 0)
        buffer[i++] = '0';
 
    // If base is 10 and value is negative, the resulting string 
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10)
        buffer[i++] = '-';
 
    buffer[i] = '\0'; // null terminate string
 
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

#include "controller.h"
#include <tinycrypt/constants.h>
//#include <test_utils.h>
#include <tinycrypt/utils.h>
#include <tinycrypt/cbc_mode.h>
#include <tinycrypt/hmac.h>
#include <tinycrypt/sha256.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>


uint8_t key[16] = { "0123456789abcdef"};

const uint8_t iv[16] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f
};

unsigned long msgCount = 0;

/* #ifdef EXAMPLE_AES
#include "aes.h"

char int2char(uint8_t i) {
  char *hex = "0123456789abcdef";
  return hex[i & 0xf];
}
#endif */

#define send_str(M) send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, strlen(M), M)
#define BLOCK_SIZE 16

// message buffer
char buf[SCEWL_MAX_DATA_SZ];


int read_msg(intf_t *intf, char *data, scewl_id_t *src_id, scewl_id_t *tgt_id,
             size_t n, int blocking) {
  scewl_hdr_t hdr;
  int read, max;

  // clear buffer and header
  memset(&hdr, 0, sizeof(hdr));
  memset(data, 0, n);

  // find header start
  do {
    hdr.magicC = 0;

    if (intf_read(intf, (char *)&hdr.magicS, 1, blocking) == INTF_NO_DATA) {
      return SCEWL_NO_MSG;
    }

    // check for SC
    if (hdr.magicS == 'S') {
      do {
        if (intf_read(intf, (char *)&hdr.magicC, 1, blocking) == INTF_NO_DATA) {
          return SCEWL_NO_MSG;
        }
      } while (hdr.magicC == 'S'); // in case of multiple 'S's in a row
    }
  } while (hdr.magicS != 'S' && hdr.magicC != 'C');

  // read rest of header
  read = intf_read(intf, (char *)&hdr + 2, sizeof(scewl_hdr_t) - 2, blocking);
  if(read == INTF_NO_DATA) {
    return SCEWL_NO_MSG;
  }

  // unpack header
  *src_id = hdr.src_id;
  *tgt_id = hdr.tgt_id;

  // read body
  max = hdr.len < n ? hdr.len : n;
  read = intf_read(intf, data, max, blocking);

  // throw away rest of message if too long
  for (int i = 0; hdr.len > max && i < hdr.len - max; i++) {
    intf_readb(intf, 0);
  }

  // report if not blocking and full message not received
  if(read == INTF_NO_DATA || read < max) {
    return SCEWL_NO_MSG;
  }

  return max;
}

int send_msg(intf_t *intf, scewl_id_t src_id, scewl_id_t tgt_id, uint16_t len, char *data) {
  scewl_hdr_t hdr;

  // pack header
  hdr.magicS  = 'S';
  hdr.magicC  = 'C';
  hdr.src_id = src_id;
  hdr.tgt_id = tgt_id;
  hdr.len    = len;
  if (hdr.tgt_id != SCEWL_FAA_ID) msgCount++;

  // send header
  intf_write(intf, (char *)&hdr, sizeof(scewl_hdr_t));

  // send body
  intf_write(intf, data, len);

  return SCEWL_OK;
}



int handle_scewl_recv(char* data, scewl_id_t src_id, uint16_t len) {
  //send_str("recieved message:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, len , data);
  char test[16];
  send_str("message Count - reciever: ");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, 2, itoa(msgCount, test, 10));
  
  uint16_t n = len - 32;
  uint8_t encrypted[n];
  uint8_t hmac[32];
  int i;
  for (i = 0; i < n; i++) encrypted[i] = data[i];
  for (i = n; i < n + 32; i++) hmac[i - n] = data[i];

  //send_str("recieved HMAC:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, 32, (char *)hmac);

  struct tc_hmac_state_struct h;
  uint8_t digest[32];
  (void)memset(&h, 0x00, sizeof(h));
  (void)tc_hmac_set_key(&h, key, sizeof(key));
  (void)tc_hmac_init(&h);
  (void)tc_hmac_update(&h, (char *)encrypted, n);
  (void)tc_hmac_final(digest, 32, &h);

  //send_str("calulated HMAC:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, 32, (char *)digest);

  if (!_compare(digest, hmac, 32))
  {
      send_str("HMAC matches, message authentic. Decrypting");

      struct tc_aes_key_sched_struct a;
      uint16_t sizeofDec = n - 16;
      uint8_t decrypted[sizeofDec];
      char *p;
      //unsigned int length;
      (void)tc_aes128_set_decrypt_key(&a, key);
      p = &data[16];
      //length = ((unsigned int) sizeof(data));
      tc_cbc_mode_decrypt(decrypted, len, (uint8_t *)p, len, (uint8_t *)data, &a);
      //send_str("decrypted message:");
      //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeofDec, (char *)decrypted);

  for (i = sizeofDec - 1; decrypted[i] == '#'; i--,sizeofDec--) decrypted[i] = '\0';
  send_str("Unpadded message:");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeofDec , (char *)decrypted); 

      return send_msg(CPU_INTF, src_id, SCEWL_ID, sizeofDec, (char *)decrypted);
  }
  else
  {
    send_str("HMAC did not match, message not authentic. Not decrypting");
    return 0;
  }  

}


int handle_scewl_send(char* data, scewl_id_t tgt_id, uint16_t len) {
  send_str("origional message:");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, len , data);
  char test[16];
  send_str("message Count - sender: ");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, 2, itoa(msgCount, test, 10));

  if (len % 16 != 0) 
  {
       for (int i = len; i < len + (16 - (len % 16)); i++) data[i] = '#';
       len = strlen(data);
        //send_str("padded message:");
      //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, len , data);     
  }

  struct tc_aes_key_sched_struct a;
	uint8_t iv_buffer[16];
  uint16_t sizeofEnc = len + 16;
	uint8_t encrypted[sizeofEnc];
  (void)tc_aes128_set_encrypt_key(&a, key);
	(void)memcpy(iv_buffer, iv, 16);
  tc_cbc_mode_encrypt(encrypted, sizeofEnc,
	(uint8_t *)data, len , iv_buffer, &a);
  //send_str("encrypted message:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeofEnc, (char *)encrypted);

  struct tc_hmac_state_struct h;
  uint8_t digest[32];
  (void)memset(&h, 0x00, sizeof(h));
  (void)tc_hmac_set_key(&h, key, sizeof(key));
  (void)tc_hmac_init(&h);
  (void)tc_hmac_update(&h, (char *)encrypted, sizeofEnc);
  (void)tc_hmac_final(digest, 32, &h);
  //send_str("HMAC:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeof(digest), (char *)digest);


  uint8_t msg[sizeofEnc + 32];
  int i;
  for (i = 0; i < sizeofEnc; i++) msg[i] = encrypted[i];
  for (i = sizeofEnc; i < sizeofEnc + 32; i++) msg[i] = digest[i - sizeofEnc];

  //send_str("combined:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeof(msg), (char *)msg);

  return send_msg(RAD_INTF, SCEWL_ID, tgt_id, sizeof(msg), (char *)msg);
}


int handle_brdcst_recv(char* data, scewl_id_t src_id, uint16_t len) {
  //send_str("recieved message:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, len , data);
  
  uint16_t n = len - 32;
  uint8_t encrypted[n];
  uint8_t hmac[32];
  int i;
  for (i = 0; i < n; i++) encrypted[i] = data[i];
  for (i = n; i < n + 32; i++) hmac[i - n] = data[i];

  //send_str("recieved HMAC:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, 32, (char *)hmac);

  struct tc_hmac_state_struct h;
  uint8_t digest[32];
  (void)memset(&h, 0x00, sizeof(h));
  (void)tc_hmac_set_key(&h, key, sizeof(key));
  (void)tc_hmac_init(&h);
  (void)tc_hmac_update(&h, (char *)encrypted, n);
  (void)tc_hmac_final(digest, 32, &h);

  //send_str("calulated HMAC:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, 32, (char *)digest);

  if (!_compare(digest, hmac, 32))
  {
      send_str("HMAC matches, message authentic. Decrypting");

      struct tc_aes_key_sched_struct a;
      uint16_t sizeofDec = n - 16;
      uint8_t decrypted[sizeofDec];
      char *p;
      (void)tc_aes128_set_decrypt_key(&a, key);
      p = &data[16];
      tc_cbc_mode_decrypt(decrypted, len, (uint8_t *)p, len, (uint8_t *)data, &a);
      //send_str("decrypted message:");
      //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeofDec, (char *)decrypted);

      for (i = sizeofDec - 1; decrypted[i] == '#'; i--,sizeofDec--) decrypted[i] = '\0';
      send_str("Unpadded message:");
      send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeofDec , (char *)decrypted); 

      return send_msg(CPU_INTF, src_id, SCEWL_BRDCST_ID, sizeofDec, (char *)decrypted);
  }
  else
  {
    send_str("HMAC did not match, message not authentic. Not decrypting");
    return 0;
  }  
}


int handle_brdcst_send(char *data, uint16_t len) {
  send_str("origional message:");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, len , data);

  if (len % 16 != 0) 
  {
       for (int i = len; i < len + (16 - (len % 16)); i++) data[i] = '#';
       len = strlen(data);
        //send_str("padded message:");
      //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, len , data);     
  }

  struct tc_aes_key_sched_struct a;
	uint8_t iv_buffer[16];
  uint16_t sizeofEnc = len + 16;
	uint8_t encrypted[sizeofEnc];
  (void)tc_aes128_set_encrypt_key(&a, key);
	(void)memcpy(iv_buffer, iv, 16);
  tc_cbc_mode_encrypt(encrypted, sizeofEnc,
	(uint8_t *)data, len , iv_buffer, &a);
  //send_str("encrypted message:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeofEnc, (char *)encrypted);

  struct tc_hmac_state_struct h;
  uint8_t digest[32];
  (void)memset(&h, 0x00, sizeof(h));
  (void)tc_hmac_set_key(&h, key, sizeof(key));
  (void)tc_hmac_init(&h);
  (void)tc_hmac_update(&h, (char *)encrypted, sizeofEnc);
  (void)tc_hmac_final(digest, 32, &h);
  //send_str("HMAC:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeof(digest), (char *)digest);

  uint8_t msg[sizeofEnc + 32];
  int i;
  for (i = 0; i < sizeofEnc; i++) msg[i] = encrypted[i];
  for (i = sizeofEnc; i < sizeofEnc + 32; i++) msg[i] = digest[i - sizeofEnc];

  //send_str("combined:");
  //send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeof(msg), (char *)msg);

  return send_msg(RAD_INTF, SCEWL_ID, SCEWL_BRDCST_ID, sizeof(msg), (char *)msg);
}   


int handle_faa_recv(char* data, uint16_t len) {
  return send_msg(CPU_INTF, SCEWL_FAA_ID, SCEWL_ID, len, data);
}


int handle_faa_send(char* data, uint16_t len) {
  return send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, len, data);
}


int handle_registration(char* msg) {
  scewl_sss_msg_t *sss_msg = (scewl_sss_msg_t *)msg;
  if (sss_msg->op == SCEWL_SSS_REG) {
    return sss_register();
  }
  else if (sss_msg->op == SCEWL_SSS_DEREG) {
    return sss_deregister();
  }

  // bad op
  return 0;
}


int sss_register() {
  char msg2[20];
  scewl_sss_msg_t msg;
  scewl_id_t src_id, tgt_id;
  int status, len;

  send_str("Secret Passcode: ");
   char secret[20];
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, 10, itoa(SECRET, secret, 10));
    send_str("SED Registration Number: ");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, 10, itoa(DATA1, secret, 10));

  // fill registration message
  msg.dev_id = SCEWL_ID;
  msg.op = SCEWL_SSS_REG;
  msg.passcode = SECRET;
  msg.serialNum = DATA1;
  
  // send registration
  status = send_msg(SSS_INTF, SCEWL_ID, SCEWL_SSS_ID, sizeof(msg), (char *)&msg);
  if (status == SCEWL_ERR) {
    return 0;
  }

  // receive response
  len = read_msg(SSS_INTF, msg2, &src_id, &tgt_id, sizeof(msg2) , 1);
  for (int i = 0; i < 16; i++) key[i] = msg2[sizeof(msg2) - 16 + i];
  send_str("SSS registration complete, Recieved secret key:");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeof(key), (char *)key);

  // notify CPU of response
  status = send_msg(CPU_INTF, src_id, tgt_id, len, msg2);
  if (status == SCEWL_ERR) {
    return 0;
  }

  // op should be REG on success
  return msg.op == SCEWL_SSS_REG;
}


int sss_deregister() {
  scewl_sss_msg_t msg;
  scewl_id_t src_id, tgt_id;
  int status, len;

  // fill registration message
  msg.dev_id = SCEWL_ID;
  msg.op = SCEWL_SSS_DEREG;
  
  // send registration
  status = send_msg(SSS_INTF, SCEWL_ID, SCEWL_SSS_ID, sizeof(msg), (char *)&msg);
  if (status == SCEWL_ERR) {
    return 0;
  }

  // receive response
  len = read_msg(SSS_INTF, (char *)&msg, &src_id, &tgt_id, sizeof(scewl_sss_msg_t), 1);

  // notify CPU of response
  status = send_msg(CPU_INTF, src_id, tgt_id, len, (char *)&msg);
  if (status == SCEWL_ERR) {
    return 0;
  }

  // op should be DEREG on success
  return msg.op == SCEWL_SSS_DEREG;
}

int main() {
  int registered = 0, len;
  scewl_hdr_t hdr;
  uint16_t src_id, tgt_id;

  // initialize interfaces
  intf_init(CPU_INTF);
  intf_init(SSS_INTF);
  intf_init(RAD_INTF);

//#ifdef EXAMPLE_AES
/*
 const uint8_t key[16] = {
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88,
	0x09, 0xcf, 0x4f, 0x3c
};

const uint8_t iv[16] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f
};

const uint8_t plaintext[128] = { "The encryption algorithm processes the plaintext, and the MAC then hashes the encrypted message to authenticate. So cool right??"
};

 struct tc_aes_key_sched_struct a;
	uint8_t iv_buffer[16];
	uint8_t encrypted[144];
	uint8_t decrypted[128];
	uint8_t *p;
	unsigned int length;
  struct tc_hmac_state_struct h;
        uint8_t digest[32];
	//int result = 0;
	(void)tc_aes128_set_encrypt_key(&a, key);

	(void)memcpy(iv_buffer, iv, 16);

	//printf("Plaintext = %s\n", plaintext);
  send_str("Plaintext message:");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeof(plaintext), (char *)plaintext);
	tc_cbc_mode_encrypt(encrypted, sizeof(plaintext) + 16,
				plaintext, sizeof(plaintext), iv_buffer, &a);
    send_str("Encrypted message:");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeof(plaintext), (char *)encrypted);
	//show_str1("encrypted = ", encrypted, 144);
        (void)memset(&h, 0x00, sizeof(h));
        (void)tc_hmac_set_key(&h, key, sizeof(key));
        (void)tc_hmac_init(&h);
        (void)tc_hmac_update(&h, plaintext, sizeof(plaintext));
        (void)tc_hmac_final(digest, 32, &h);
  send_str("MAC message:");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeof(digest), (char *)digest);

	(void)tc_aes128_set_decrypt_key(&a, key);
	p = &encrypted[16];
	length = ((unsigned int) sizeof(encrypted));
	tc_cbc_mode_decrypt(decrypted, length, p, length, encrypted, &a);
    send_str("Decrypted message:");
  send_msg(RAD_INTF, SCEWL_ID, SCEWL_FAA_ID, sizeof(plaintext), (char *)decrypted);
	//printf("Decrypted = %s\n", decrypted);
  */
//#endif

  // serve forever
  while (1) {
    // register with SSS
    read_msg(CPU_INTF, buf, &hdr.src_id, &hdr.tgt_id, sizeof(buf), 1);

    if (hdr.tgt_id == SCEWL_SSS_ID) {
      registered = handle_registration(buf);
    }

    // server while registered
    while (registered) {
      memset(&hdr, 0, sizeof(hdr));

      // handle outgoing message from CPU
      if (intf_avail(CPU_INTF)) {
        // Read message from CPU
        len = read_msg(CPU_INTF, buf, &src_id, &tgt_id, sizeof(buf), 1);

        if (tgt_id == SCEWL_BRDCST_ID) {
          handle_brdcst_send(buf, len);
        } else if (tgt_id == SCEWL_SSS_ID) {
          registered = handle_registration(buf);
        } else if (tgt_id == SCEWL_FAA_ID) {
          handle_faa_send(buf, len);
        } else {
          handle_scewl_send(buf, tgt_id, len);
        }

        continue;
      }

      // handle incoming radio message
      if (intf_avail(RAD_INTF)) {
        // Read message from antenna
        len = read_msg(RAD_INTF, buf, &src_id, &tgt_id, sizeof(buf), 1);

        if (src_id != SCEWL_ID) { // ignore our own outgoing messages
          if (tgt_id == SCEWL_BRDCST_ID) {
            // receive broadcast message
            handle_brdcst_recv(buf, src_id, len);
          } else if (tgt_id == SCEWL_ID) {
            // receive unicast message
            if (src_id == SCEWL_FAA_ID) {
              handle_faa_recv(buf, len);
            } else {
              handle_scewl_recv(buf, src_id, len);
            }
          }
        }
      }
    }
  }
}
//Zero Proof for authenication
//OAUTH (initial password, then server challenges drones)
//strncpy over strcpy - explicity define num of characters to prevent overflow
/*
Response/challenge means of authentication

To Do:
-pull in new changes from mitre server

-only send key to drones on proper registration, not on degistration or if already registered.
currently sends to all.

-different key for HMAC and AES
-Random IV
-Add random first block to encryption

-Check authenticity of drone in supply chain before distributing key
-counter or timer (timestamp included in message, only approved in small time window)

-Next, we want to remind all teams of one critical security feature. 
All attacking teams will receive the compiled binary firmware from one UAV’s SCEWL Bus Controller,
providing access to any secrets compiled into the device. 
After the organizers collect the binary, we will run make remove_sed to remove that device 
from the deployment, so make sure to do any necessary cleanup there 
(in dockerfiles/3_remove_sed.Dockerfile) necessary to protect your system from the 
compromised SED.

-compromised CPU does not result in a compromised controller (ie no blocking?)

-Random key generation in SSS post prossessing and distributing key
to drones on registration (done)
-Proper comparing of MACs (done)

-Address buffer overflow attacks (check the size of any input read, 
reject any message too large) *memset *memcpy
-Address side-chain attacks
-Address FAA attacks, must be passed directly to the CPU (nothing to do)

-dynamic or static memory for message buffer
-validate data before putting in buffer
-UNIX Sockets - how does our communications/exchange of communications work?

-potenitally modifying build process to address vulnerabilities (safegaurds to gcc compiler)
*/