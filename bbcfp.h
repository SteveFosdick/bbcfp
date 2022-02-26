#ifndef BBCFP_INC
#define BBCFP_INC

#include <stdbool.h>
#include <stdio.h>

extern double bbcfp2d(unsigned char *bbcfp);
extern bool d2bbcfp(double value, unsigned char *bbcfp);
extern bool write_bbcfp(double value, FILE *fp);

#endif
