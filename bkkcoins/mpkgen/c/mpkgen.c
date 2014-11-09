// Generate Bitcoin address from Electrum MPK and seq number
//
// Build with: gcc -o mpkgen mpkgen.c -lcrypto
//
// some code here derived from vanitygen keyconv utility
// git://github.com/samr7/vanitygen.git

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <openssl/evp.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>

#if !defined(_WIN32)
#include <unistd.h>
#else
#include "winglue.h"
#endif

const char *b58alphabet = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

void b58encode_check(void *buf, size_t len, char *result)
{
	unsigned char hash1[32];
	unsigned char hash2[32];

	int d, p;

	BN_CTX *bnctx;
	BIGNUM *bn, *bndiv, *bntmp;
	BIGNUM bna, bnb, bnbase, bnrem;
	unsigned char *binres;
	int brlen, zpfx;

	bnctx = BN_CTX_new();
	BN_init(&bna);
	BN_init(&bnb);
	BN_init(&bnbase);
	BN_init(&bnrem);
	BN_set_word(&bnbase, 58);

	bn = &bna;
	bndiv = &bnb;

	brlen = (2 * len) + 4;
	binres = (unsigned char*) malloc(brlen);
	memcpy(binres, buf, len);

	SHA256(binres, len, hash1);
	SHA256(hash1, sizeof(hash1), hash2);
	memcpy(&binres[len], hash2, 4);

	BN_bin2bn(binres, len + 4, bn);

	for (zpfx = 0; zpfx < (len + 4) && binres[zpfx] == 0; zpfx++);

	p = brlen;
	while (!BN_is_zero(bn)) {
		BN_div(bndiv, &bnrem, bn, &bnbase, bnctx);
		bntmp = bn;
		bn = bndiv;
		bndiv = bntmp;
		d = BN_get_word(&bnrem);
		binres[--p] = b58alphabet[d];
	}

	while (zpfx--) {
		binres[--p] = b58alphabet[0];
	}

	memcpy(result, &binres[p], brlen - p);
	result[brlen - p] = '\0';

	free(binres);
	BN_clear_free(&bna);
	BN_clear_free(&bnb);
	BN_clear_free(&bnbase);
	BN_clear_free(&bnrem);
	BN_CTX_free(bnctx);
}

void b58encode_address(const EC_POINT *ppoint, const EC_GROUP *pgroup, int addrtype, char *result)
{
        unsigned char eckey_buf[128], *pend;
        unsigned char binres[21] = {0,};
        unsigned char hash1[32];

        pend = eckey_buf;

        EC_POINT_point2oct(pgroup,
                           ppoint,
                           POINT_CONVERSION_UNCOMPRESSED,
                           eckey_buf,
                           sizeof(eckey_buf),
                           NULL);
        pend = eckey_buf + 0x41;
        binres[0] = addrtype;
        SHA256(eckey_buf, pend - eckey_buf, hash1);
        RIPEMD160(hash1, sizeof(hash1), &binres[1]);

        b58encode_check(binres, sizeof(binres), result);
}

unsigned char hex(unsigned char c)
{
	return (c <= '9') ? c-'0' : c-'a'+10;
}

int main(int argc, char **argv)
{
	unsigned char ecprot[128];
	unsigned char pbuf[1024];
	EC_KEY *pkey;
	EC_POINT *ptnew, *ptmpk, *ptz;
	BIGNUM z;
	unsigned char *pend = (unsigned char *) pbuf, *px;
	unsigned char hash1[32], hashz[32];
	int n, zlen;
	
	if(argc < 3) {
		printf("Usage: %s <mpk> <seq>\n", argv[0]);
		exit(1);
		}

	OpenSSL_add_all_algorithms();
	
	strcpy(pbuf, argv[2]);
	strcat(pbuf, ":0:");
	zlen = strlen(pbuf);
	px = pbuf + zlen;
	for(n=0; n < 64; n++)
		px[n] = hex(argv[1][n*2])*16 + hex(argv[1][n*2+1]);
	SHA256(pbuf, zlen+64, hash1);
	SHA256(hash1, sizeof(hash1), hashz);
	BN_init(&z);
	BN_bin2bn(hashz, 32, &z);
	
	pkey = EC_KEY_new_by_curve_name(NID_secp256k1);
	ptmpk = EC_POINT_new(EC_KEY_get0_group(pkey));
	pbuf[zlen-1] = 0x04;
	EC_POINT_oct2point(EC_KEY_get0_group(pkey), ptmpk, pbuf+zlen-1, 65, NULL);
	ptz = EC_POINT_new(EC_KEY_get0_group(pkey));
	EC_POINT_mul(EC_KEY_get0_group(pkey), ptz, NULL, EC_GROUP_get0_generator(EC_KEY_get0_group(pkey)), &z, NULL);
	ptnew = EC_POINT_new(EC_KEY_get0_group(pkey));
	EC_POINT_add(EC_KEY_get0_group(pkey), ptnew, ptmpk, ptz, NULL);
	EC_KEY_set_public_key(pkey, ptnew);
	i2o_ECPublicKey(pkey, &pend);
	b58encode_address(EC_KEY_get0_public_key(pkey), EC_KEY_get0_group(pkey), 0, ecprot);
	printf("%s\n", ecprot);

}

