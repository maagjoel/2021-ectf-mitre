/*  test_hmac.c - TinyCrypt implementation of some HMAC tests */

/*
 *  Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*
  DESCRIPTION
  This module tests the following HMAC routines:

  Scenarios tested include:
  - HMAC tests (RFC 4231 test vectors)
*/


//#include <tinycrypt/constants.h>
// #include <test_utils.h>

//#include <tinycrypt/hmac.h>
//#include <tinycrypt/sha256.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
/*
 * NIST test vectors for encryption.
 */
/*

unsigned int do_hmac_test(TCHmacState_t h, unsigned int testnum, const uint8_t *data,
		          size_t datalen, const uint8_t *expected,
		          size_t expectedlen)
{
        uint8_t digest[32];
        unsigned int result = TC_PASS;

        (void)tc_hmac_init(h);
        (void)tc_hmac_update(h, data, datalen);
        (void)tc_hmac_final(digest, TC_SHA256_DIGEST_SIZE, h);
        result = check_result(testnum, expected, expectedlen,
			      digest, sizeof(digest));
        return result;
}


unsigned int test_1(void)
{
    
        const uint8_t expected[32] = {
		0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53, 0x5c, 0xa8, 0xaf, 0xce,
		0xaf, 0x0b, 0xf1, 0x2b, 0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83, 0x3d, 0xa7,
		0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7
        };

        const uint8_t key[20] = {
		0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
		0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
        };
        
        //result = check_result(testnum, expected, expectedlen, digest, sizeof(digest));
        //result = do_hmac_test(&h, 1, data, sizeof(data),expected, sizeof(expected));
        unsigned int result = TC_PASS;


        TC_PRINT("HMAC %s:\n", __func__);
        
        uint8_t digest[32];
        const uint8_t key[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88,
        0x09, 0xcf, 0x4f, 0x3c
       };

        const uint8_t data[8] = "hellooo1";

        struct tc_hmac_state_struct h;

        (void)memset(&h, 0x00, sizeof(h));
        (void)tc_hmac_set_key(&h, key, sizeof(key));
        (void)tc_hmac_init(&h);
        (void)tc_hmac_update(&h, data, sizeof(data));
        (void)tc_hmac_final(digest, 32, &h); // TC_SHA256_DIGEST_SIZE = 32

        show_str("Digest", digest, sizeof(digest));

        return result;
}
*/
/*
unsigned int test_2(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("HMAC %s:\n", __func__);
        const uint8_t key[4] = {
                0x4a, 0x65, 0x66, 0x65
        };
        const uint8_t data[28] = {
		0x77, 0x68, 0x61, 0x74, 0x20, 0x64, 0x6f, 0x20, 0x79, 0x61, 0x20, 0x77,
		0x61, 0x6e, 0x74, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x6e, 0x6f, 0x74, 0x68,
		0x69, 0x6e, 0x67, 0x3f
        };
        const uint8_t expected[32] = {
		0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e, 0x6a, 0x04, 0x24, 0x26,
		0x08, 0x95, 0x75, 0xc7, 0x5a, 0x00, 0x3f, 0x08, 0x9d, 0x27, 0x39, 0x83,
		0x9d, 0xec, 0x58, 0xb9, 0x64, 0xec, 0x38, 0x43
        };
        struct tc_hmac_state_struct h;

        (void)memset(&h, 0x00, sizeof(h));
        (void)tc_hmac_set_key(&h, key, sizeof(key));

        result = do_hmac_test(&h, 2, data, sizeof(data), expected,
			      sizeof(expected));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_3(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("HMAC %s:\n", __func__);
        const uint8_t key[20] = {
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
        };
        const uint8_t data[50] = {
		0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
		0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
		0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
		0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
		0xdd, 0xdd
        };
        const uint8_t expected[32] = {
		0x77, 0x3e, 0xa9, 0x1e, 0x36, 0x80, 0x0e, 0x46, 0x85, 0x4d, 0xb8, 0xeb,
		0xd0, 0x91, 0x81, 0xa7, 0x29, 0x59, 0x09, 0x8b, 0x3e, 0xf8, 0xc1, 0x22,
		0xd9, 0x63, 0x55, 0x14, 0xce, 0xd5, 0x65, 0xfe
        };
        struct tc_hmac_state_struct h;

        (void)memset(&h, 0x00, sizeof(h));
        (void)tc_hmac_set_key(&h, key, sizeof(key));

        result = do_hmac_test(&h, 3, data, sizeof(data), expected,
			      sizeof(expected));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_4(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("HMAC %s:\n", __func__);

        const uint8_t key[25] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
		0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
		0x19
        };
        const uint8_t data[50] = {
		0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
		0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
		0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
		0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
		0xcd, 0xcd
        };
        const uint8_t expected[32] = {
		0x82, 0x55, 0x8a, 0x38, 0x9a, 0x44, 0x3c, 0x0e, 0xa4, 0xcc, 0x81, 0x98,
		0x99, 0xf2, 0x08, 0x3a, 0x85, 0xf0, 0xfa, 0xa3, 0xe5, 0x78, 0xf8, 0x07,
		0x7a, 0x2e, 0x3f, 0xf4, 0x67, 0x29, 0x66, 0x5b
        };
        struct tc_hmac_state_struct h;

        (void)memset(&h, 0x00, sizeof(h));
        (void)tc_hmac_set_key(&h, key, sizeof(key));

        result = do_hmac_test(&h, 4, data, sizeof(data), expected,
			      sizeof(expected));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_5(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("HMAC %s:\n", __func__);
        const uint8_t key[20] = {
		0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
		0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c
        };
        const uint8_t data[20] = {
		0x54, 0x65, 0x73, 0x74, 0x20, 0x57, 0x69, 0x74, 0x68, 0x20, 0x54, 0x72,
		0x75, 0x6e, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e
        };
        const uint8_t expected[32] = {
		0xa3, 0xb6, 0x16, 0x74, 0x73, 0x10, 0x0e, 0xe0, 0x6e, 0x0c, 0x79, 0x6c,
		0x29, 0x55, 0x55, 0x2b, 0xfa, 0x6f, 0x7c, 0x0a, 0x6a, 0x8a, 0xef, 0x8b,
		0x93, 0xf8, 0x60, 0xaa, 0xb0, 0xcd, 0x20, 0xc5
        };
        struct tc_hmac_state_struct h;

        (void)memset(&h, 0x00, sizeof(h));
        (void)tc_hmac_set_key(&h, key, sizeof(key));

        result = do_hmac_test(&h, 5, data, sizeof(data), expected,
			      sizeof(expected));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_6(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("HMAC %s:\n", __func__);
        const uint8_t key[131] = {
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
        };
        const uint8_t data[54] = {
		0x54, 0x65, 0x73, 0x74, 0x20, 0x55, 0x73, 0x69, 0x6e, 0x67, 0x20, 0x4c,
		0x61, 0x72, 0x67, 0x65, 0x72, 0x20, 0x54, 0x68, 0x61, 0x6e, 0x20, 0x42,
		0x6c, 0x6f, 0x63, 0x6b, 0x2d, 0x53, 0x69, 0x7a, 0x65, 0x20, 0x4b, 0x65,
		0x79, 0x20, 0x2d, 0x20, 0x48, 0x61, 0x73, 0x68, 0x20, 0x4b, 0x65, 0x79,
		0x20, 0x46, 0x69, 0x72, 0x73, 0x74
        };
        const uint8_t expected[32] = {
		0x60, 0xe4, 0x31, 0x59, 0x1e, 0xe0, 0xb6, 0x7f, 0x0d, 0x8a, 0x26, 0xaa,
		0xcb, 0xf5, 0xb7, 0x7f, 0x8e, 0x0b, 0xc6, 0x21, 0x37, 0x28, 0xc5, 0x14,
		0x05, 0x46, 0x04, 0x0f, 0x0e, 0xe3, 0x7f, 0x54
        };
        struct tc_hmac_state_struct h;

        (void)memset(&h, 0x00, sizeof(h));
        (void)tc_hmac_set_key(&h, key, sizeof(key));

        result = do_hmac_test(&h, 6, data, sizeof(data), expected,
			      sizeof(expected));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_7(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("HMAC %s:\n", __func__);
        const uint8_t key[131] = {
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
        };
        const uint8_t data[152] = {
		0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x74, 0x65,
		0x73, 0x74, 0x20, 0x75, 0x73, 0x69, 0x6e, 0x67, 0x20, 0x61, 0x20, 0x6c,
		0x61, 0x72, 0x67, 0x65, 0x72, 0x20, 0x74, 0x68, 0x61, 0x6e, 0x20, 0x62,
		0x6c, 0x6f, 0x63, 0x6b, 0x2d, 0x73, 0x69, 0x7a, 0x65, 0x20, 0x6b, 0x65,
		0x79, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x61, 0x20, 0x6c, 0x61, 0x72, 0x67,
		0x65, 0x72, 0x20, 0x74, 0x68, 0x61, 0x6e, 0x20, 0x62, 0x6c, 0x6f, 0x63,
		0x6b, 0x2d, 0x73, 0x69, 0x7a, 0x65, 0x20, 0x64, 0x61, 0x74, 0x61, 0x2e,
		0x20, 0x54, 0x68, 0x65, 0x20, 0x6b, 0x65, 0x79, 0x20, 0x6e, 0x65, 0x65,
		0x64, 0x73, 0x20, 0x74, 0x6f, 0x20, 0x62, 0x65, 0x20, 0x68, 0x61, 0x73,
		0x68, 0x65, 0x64, 0x20, 0x62, 0x65, 0x66, 0x6f, 0x72, 0x65, 0x20, 0x62,
		0x65, 0x69, 0x6e, 0x67, 0x20, 0x75, 0x73, 0x65, 0x64, 0x20, 0x62, 0x79,
		0x20, 0x74, 0x68, 0x65, 0x20, 0x48, 0x4d, 0x41, 0x43, 0x20, 0x61, 0x6c,
		0x67, 0x6f, 0x72, 0x69, 0x74, 0x68, 0x6d, 0x2e
        };
        const uint8_t expected[32] = {
		0x9b, 0x09, 0xff, 0xa7, 0x1b, 0x94, 0x2f, 0xcb, 0x27, 0x63, 0x5f, 0xbc,
		0xd5, 0xb0, 0xe9, 0x44, 0xbf, 0xdc, 0x63, 0x64, 0x4f, 0x07, 0x13, 0x93,
		0x8a, 0x7f, 0x51, 0x53, 0x5c, 0x3a, 0x35, 0xe2
        };
        struct tc_hmac_state_struct h;

        (void)memset(&h, 0x00, sizeof(h));
        (void)tc_hmac_set_key(&h, key, sizeof(key));

        result = do_hmac_test(&h, 7, data, sizeof(data), expected,
			      sizeof(expected));
        TC_END_RESULT(result);
        return result;
}
*/
/*
 * Main task to test AES
 */
#ifndef __TC_HMAC_H__
#define __TC_HMAC_H__

#include <tinycrypt/sha256.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tc_hmac_state_struct {
	/* the internal state required by h */
	struct tc_sha256_state_struct hash_state;
	/* HMAC key schedule */
	uint8_t key[2*TC_SHA256_BLOCK_SIZE];
};
typedef struct tc_hmac_state_struct *TCHmacState_t;

/**
 *  @brief HMAC set key procedure
 *  Configures ctx to use key
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if
 *                ctx == NULL or
 *                key == NULL or
 *                key_size == 0
 * @param ctx IN/OUT -- the struct tc_hmac_state_struct to initial
 * @param key IN -- the HMAC key to configure
 * @param key_size IN -- the HMAC key size
 */
int tc_hmac_set_key(TCHmacState_t ctx, const uint8_t *key,
		    unsigned int key_size);

/**
 * @brief HMAC init procedure
 * Initializes ctx to begin the next HMAC operation
 * @return returns TC_CRYPTO_SUCCESS (1)
 *         returns TC_CRYPTO_FAIL (0) if: ctx == NULL or key == NULL
 * @param ctx IN/OUT -- struct tc_hmac_state_struct buffer to init
 */
int tc_hmac_init(TCHmacState_t ctx);

/**
 *  @brief HMAC update procedure
 *  Mixes data_length bytes addressed by data into state
 *  @return returns TC_CRYPTO_SUCCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if: ctx == NULL or key == NULL
 *  @note Assumes state has been initialized by tc_hmac_init
 *  @param ctx IN/OUT -- state of HMAC computation so far
 *  @param data IN -- data to incorporate into state
 *  @param data_length IN -- size of data in bytes
 */
int tc_hmac_update(TCHmacState_t ctx, const void *data,
		   unsigned int data_length);

/**
 *  @brief HMAC final procedure
 *  Writes the HMAC tag into the tag buffer
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                tag == NULL or
 *                ctx == NULL or
 *                key == NULL or
 *                taglen != TC_SHA256_DIGEST_SIZE
 *  @note ctx is erased before exiting. This should never be changed/removed.
 *  @note Assumes the tag bufer is at least sizeof(hmac_tag_size(state)) bytes
 *  state has been initialized by tc_hmac_init
 *  @param tag IN/OUT -- buffer to receive computed HMAC tag
 *  @param taglen IN -- size of tag in bytes
 *  @param ctx IN/OUT -- the HMAC state for computing tag
 */
int tc_hmac_final(uint8_t *tag, unsigned int taglen, TCHmacState_t ctx);

#ifdef __cplusplus
}
#endif

#endif /*__TC_HMAC_H__*/

#ifndef __TC_SHA256_H__
#define __TC_SHA256_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TC_SHA256_BLOCK_SIZE (64)
#define TC_SHA256_DIGEST_SIZE (32)
#define TC_SHA256_STATE_BLOCKS (TC_SHA256_DIGEST_SIZE/4)

struct tc_sha256_state_struct {
	unsigned int iv[TC_SHA256_STATE_BLOCKS];
	uint64_t bits_hashed;
	uint8_t leftover[TC_SHA256_BLOCK_SIZE];
	size_t leftover_offset;
};

typedef struct tc_sha256_state_struct *TCSha256State_t;

/**
 *  @brief SHA256 initialization procedure
 *  Initializes s
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if s == NULL
 *  @param s Sha256 state struct
 */
int tc_sha256_init(TCSha256State_t s);

/**
 *  @brief SHA256 update procedure
 *  Hashes data_length bytes addressed by data into state s
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                s == NULL,
 *                s->iv == NULL,
 *                data == NULL
 *  @note Assumes s has been initialized by tc_sha256_init
 *  @warning The state buffer 'leftover' is left in memory after processing
 *           If your application intends to have sensitive data in this
 *           buffer, remind to erase it after the data has been processed
 *  @param s Sha256 state struct
 *  @param data message to hash
 *  @param datalen length of message to hash
 */
int tc_sha256_update (TCSha256State_t s, const uint8_t *data, size_t datalen);

/**
 *  @brief SHA256 final procedure
 *  Inserts the completed hash computation into digest
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if:
 *                s == NULL,
 *                s->iv == NULL,
 *                digest == NULL
 *  @note Assumes: s has been initialized by tc_sha256_init
 *        digest points to at least TC_SHA256_DIGEST_SIZE bytes
 *  @warning The state buffer 'leftover' is left in memory after processing
 *           If your application intends to have sensitive data in this
 *           buffer, remind to erase it after the data has been processed
 *  @param digest unsigned eight bit integer
 *  @param Sha256 state struct
 */
int tc_sha256_final(uint8_t *digest, TCSha256State_t s);

#ifdef __cplusplus
}
#endif

#endif /* __TC_SHA256_H__ */

static inline void show_str1(const char *label, const uint8_t *s, size_t len)
{
        unsigned int i;

        printf("%s = ", label);
        for (i = 0; i < (unsigned int) len; ++i) {
                printf("%02x", s[i]);
        }
        printf("\n");
}

int main(void)
{
        

        const uint8_t key[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88,
        0x09, 0xcf, 0x4f, 0x3c
        };

        const uint8_t data[8] = "hellooo1";

        struct tc_hmac_state_struct h;
        uint8_t digest[32];

        (void)memset(&h, 0x00, sizeof(h));
        (void)tc_hmac_set_key(&h, key, sizeof(key));
        (void)tc_hmac_init(&h);
        (void)tc_hmac_update(&h, data, sizeof(data));
        (void)tc_hmac_final(digest, 32, &h); // TC_SHA256_DIGEST_SIZE = 32

        show_str1("Digest", digest, sizeof(digest));

        return 0;
}
