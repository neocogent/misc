#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include "openssl/sha.h"

#define MAX_PWD_LEN 128

int min_length = 4;
int max_length = 4;

void SaltAndHash(unsigned char *pwd, char *pattern)
{
	unsigned char buf[MAX_PWD_LEN];
	
	sprintf((char *)buf, pattern, pwd, pwd, pwd, pwd);
    //printf("%s > %s", pwd, buf);
    
    memset(pwd, 0, MAX_PWD_LEN);
	SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, buf, strlen((char *)buf));
    SHA256_Final(buf, &sha256);
 
	int n;
	for(n = 0; n < SHA256_DIGEST_LENGTH; n++)
		printf("%02x", buf[n]);
	printf("\n");
}

void InitTemplate(int nS, char *t, int nC)
{
	int n = t[0] = 0;
	memset(t+1, 1, min_length-1);
	while(nS > 0) {
		t[n++] += nS % (nC);
		nS /= (nC);
	}
}

int main(int argc, char *argv[])
{
	int n_done = 0;
	int n_limit = 0;
	int n_start = 0;
	
	char pattern[MAX_PWD_LEN];
	unsigned char charset[256];
	char pwd_template[MAX_PWD_LEN];
	char pwd[MAX_PWD_LEN];
	
	int charset_n = 1;
	
	int n, len, idx;		
	
	charset[0] = 0;
	for(n='a'; n<='z'; n++)
		charset[charset_n++] = (unsigned char)n;
	for(n='A'; n<='Z'; n++)
		charset[charset_n++] = (unsigned char)n;
	for(n='0'; n<='9'; n++)
		charset[charset_n++] = (unsigned char)n;	
	
	memset(pwd, 0, sizeof(pwd));
	memset(pwd_template, 0, sizeof(pwd_template));
	memset(pwd_template+1, 1, min_length-1);
	
	if(argc > 1) {
		n_start = atoi(argv[1]);
		InitTemplate(n_start, pwd_template, charset_n-1);
		n_limit = atoi(argv[2]);
		fprintf(stderr, "Starting at %d for %d pwds only.\n", n_start, n_limit);
		}
	
	FILE *fh = fopen("patterns", "r");
	if(!fh) {
		fprintf(stderr, "Cannot open patterns file\n");
		exit(1);
		}
	if(!fgets(pattern, MAX_PWD_LEN, fh)){
		fprintf(stderr, "No more patterns available\n");
		exit(2);
		}
	fprintf(stderr, "Starting pattern %s", pattern);

	for(;;) {
		idx = 0;
		for(;;) {
			if(n_limit > 0 && n_done >= n_limit) {
				if(!fgets(pattern, MAX_PWD_LEN, fh)) {
					fprintf(stderr, "Completed all patterns\n");
					exit(0);
					}
				fprintf(stderr, "%d passwords done\nStarting pattern %s", n_done, pattern);
				InitTemplate(n_start, pwd_template, charset_n-1);
				n_done = idx = 0;
				}
			if(pwd_template[idx] < (charset_n-1)) {
				pwd_template[idx]++;
				break;
				}
			else {
				pwd_template[idx] = 1;
				idx++;

				if(idx == max_length) {
					if(!fgets(pattern, MAX_PWD_LEN, fh)) {
						fprintf(stderr, "Completed all patterns\n");
						exit(0);
						}
					fprintf(stderr, "%d passwords done\nStarting pattern %s", n_done, pattern);
					InitTemplate(n_start, pwd_template, charset_n-1);
					n_done = idx = 0;
					}
				}
			}	
		
		len = strlen(pwd_template);
		for(n=0; n<len; n++)
			pwd[n] = charset[pwd_template[n]];
		pwd[n] = 0;
		
		SaltAndHash(pwd, pattern);
			
		n_done++;
		//if(n_done++ % 1000 == 999)
		//	fprintf(stderr, "%d,", n_done/1000);
		}

}
