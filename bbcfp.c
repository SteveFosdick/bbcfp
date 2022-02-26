#include "bbcfp.h"
#include <stdint.h>
#include <math.h>

bool d2bbcfp(double value, unsigned char *bbcfp)
{
    int exponent;
    uint_least32_t mantissa;

    if (value == 0.0) {
        exponent = 0;
        mantissa = 0;
    }
    else {
        uint_least32_t sign = 0;
        if (value < 0) {
            value = -value;
            sign = 0x80000000;
        }
        mantissa = (uint_least32_t)(frexp(value, &exponent) * 4294967296.0);
        if (exponent < -128 || exponent > 127)
            return false;
        exponent += 0x80;
        mantissa = (mantissa & 0x7fffffff)|sign;
    }
    bbcfp[0] = mantissa;
    bbcfp[1] = mantissa >> 8;
    bbcfp[2] = mantissa >> 16;
    bbcfp[3] = mantissa >> 24;
    bbcfp[4] = exponent;
    return true;
}

bool write_bbcfp(double value, FILE *fp)
{
    unsigned char buf[6];
    if (d2bbcfp(value, buf+1)) {
        buf[0] = 0xff;
        if (fwrite(buf, 6, 1, fp) == 1)
            return true;
    }
    return false;
}

double bbcfp2d(unsigned char *five)
{
    uint_least32_t mantissa = five[0] | (five[1] << 8) | (five[2] << 16) | (five[3] << 24);
    int exponent = five[4];
    if (mantissa || exponent) {
        exponent -= 0x80;
        double sign = 1;
        if (mantissa & 0x80000000)
            sign = -1;
        mantissa |= 0x80000000;
        return ldexp((double)mantissa / 4294967296.0, exponent) * sign;
    }
    return 0.0;
}
