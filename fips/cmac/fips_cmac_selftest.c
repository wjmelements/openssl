/* ====================================================================
 * Copyright (c) 2011 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#define OPENSSL_FIPSAPI

#include <string.h>
#include <openssl/err.h>
#include <openssl/fips.h>
#include <openssl/cmac.h>

#ifdef OPENSSL_FIPS
typedef struct {
	const EVP_CIPHER *(*alg)(void);
	const unsigned char key[EVP_MAX_KEY_LENGTH]; int keysize;
	const unsigned char msg[64]; int msgsize;
	const unsigned char mac[32]; int macsize;
} CMAC_KAT;

/* from http://csrc.nist.gov/publications/nistpubs/800-38B/SP_800-38B.pdf */
static const CMAC_KAT vector[] = {
    {	EVP_aes_128_cbc,	/* Example 3: Mlen = 320 */
	{ 0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,
	  0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c }, 128,
	{ 0x6b,0xc1,0xbe,0xe2, 0x2e,0x40,0x9f,0x96,
	  0xe9,0x3d,0x7e,0x11, 0x73,0x93,0x17,0x2a,
	  0xae,0x2d,0x8a,0x57, 0x1e,0x03,0xac,0x9c,
	  0x9e,0xb7,0x6f,0xac, 0x45,0xaf,0x8e,0x51,
	  0x30,0xc8,0x1c,0x46, 0xa3,0x5c,0xe4,0x11 }, 320,
	{ 0xdf,0xa6,0x67,0x47, 0xde,0x9a,0xe6,0x30,
	  0x30,0xca,0x32,0x61, 0x14,0x97,0xc8,0x27 }, 128
    },
    {	EVP_aes_192_cbc,	/* Example 5: Mlen = 0 */
	{ 0x8e,0x73,0xb0,0xf7, 0xda,0x0e,0x64,0x52,
	  0xc8,0x10,0xf3,0x2b, 0x80,0x90,0x79,0xe5,
	  0x62,0xf8,0xea,0xd2, 0x52,0x2c,0x6b,0x7b, }, 192,
	{ 0x0 }, 0,
	{ 0xd1,0x7d,0xdf,0x46, 0xad,0xaa,0xcd,0xe5,
	  0x31,0xca,0xc4,0x83, 0xde,0x7a,0x93,0x67, }, 128
    },
    {	EVP_aes_256_cbc,	/* Example 12: Mlen = 512 */
	{ 0x60,0x3d,0xeb,0x10, 0x15,0xca,0x71,0xbe,
	  0x2b,0x73,0xae,0xf0, 0x85,0x7d,0x77,0x81,
	  0x1f,0x35,0x2c,0x07, 0x3b,0x61,0x08,0xd7,
	  0x2d,0x98,0x10,0xa3, 0x09,0x14,0xdf,0xf4, }, 256,
	{ 0x6b,0xc1,0xbe,0xe2, 0x2e,0x40,0x9f,0x96,
	  0xe9,0x3d,0x7e,0x11, 0x73,0x93,0x17,0x2a,
	  0xae,0x2d,0x8a,0x57, 0x1e,0x03,0xac,0x9c,
	  0x9e,0xb7,0x6f,0xac, 0x45,0xaf,0x8e,0x51,
	  0x30,0xc8,0x1c,0x46, 0xa3,0x5c,0xe4,0x11,
	  0xe5,0xfb,0xc1,0x19, 0x1a,0x0a,0x52,0xef,
	  0xf6,0x9f,0x24,0x45, 0xdf,0x4f,0x9b,0x17,
	  0xad,0x2b,0x41,0x7b, 0xe6,0x6c,0x37,0x10, }, 512,
	{ 0xe1,0x99,0x21,0x90, 0x54,0x9f,0x6e,0xd5,
	  0x69,0x6a,0x2c,0x05, 0x6c,0x31,0x54,0x10, }, 128,
    },
# if 0
    /* Removed because the actual result was:
	  0x74,0x3d,0xdb,0xe0,  0xce,0x2d,0xc2,0xed
       I suspect an error on my part -- Richard Levitte
     */
    {	EVP_des_ede3_cbc,	/* Example 15: Mlen = 160 */
	{ 0x8a,0xa8,0x3b,0xf8, 0xcb,0xda,0x10,0x62,
	  0x0b,0xc1,0xbf,0x19, 0xfb,0xb6,0xcd,0x58,
	  0xbc,0x31,0x3d,0x4a, 0x37,0x1c,0xa8,0xb5, }, 192,
	{ 0x6b,0xc1,0xbe,0xe2, 0x2e,0x40,0x9f,0x96,
	  0xe9,0x3d,0x7e,0x11, 0x73,0x93,0x17,0x2a,
	  0xae,0x2d,0x8a,0x57, }, 160,
	{ 0xd3,0x2b,0xce,0xbe, 0x43,0xd2,0x3d,0x80, }, 64,
    },
# endif
};

int FIPS_selftest_cmac()
    {
    size_t n;
    unsigned int     outlen;
    unsigned char    out[32];
    const EVP_CIPHER *cipher;
    CMAC_CTX *ctx = CMAC_CTX_new();
    const CMAC_KAT *t;

    for(n=0,t=vector; n<sizeof(vector)/sizeof(vector[0]); n++,t++)
	{
	cipher = (*t->alg)();
	CMAC_Init(ctx, t->key, t->keysize/8, cipher, 0);
	CMAC_Update(ctx, t->msg, t->msgsize/8);
	CMAC_Final(ctx, out, &outlen);
	CMAC_CTX_cleanup(ctx);

	if(outlen != t->macsize/8 || memcmp(out,t->mac,outlen))
	    {
	    FIPSerr(FIPS_F_FIPS_SELFTEST_CMAC,FIPS_R_SELFTEST_FAILED);
	    return 0;
	    }
	}

    CMAC_CTX_free(ctx);
    return 1;
    }
#endif
