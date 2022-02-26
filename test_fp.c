#include "bbcfp.h"
#include <errno.h>
#include <string.h>
#include <math.h>

static bool read_check(const char *fn, FILE *fp, double expected)
{
    int ch = getc(fp);
    if (ch == EOF) {
        fprintf(stderr, "test-fp: unexpected EOF on %s at %ld\n", fn, ftell(fp));
        return false;
    }
    if (ch != 0xff) {
        fprintf(stderr, "test-fp: type mismatch/out of sync on %s at %ld\n", fn, ftell(fp));
        return false;
    }
    unsigned char data[5];
    if (fread(data, 5, 1, fp) != 1) {
        if (ferror(fp))
            fprintf(stderr, "test-fp: read error on %s at %ld\n", fn, ftell(fp));
        else
            fprintf(stderr, "test-fp: unexpected EOF on %s at %ld\n", fn, ftell(fp));
        return false;
    }
    double value = bbcfp2d(data);
    double error = fabs(value-expected);
    if (error > 1e-38) {
        printf("mismatch %g!=%g, error=%g on %s at %ld\n", value, expected, error, fn, ftell(fp));
        return false;
    }
    return true;
}

static bool check_series(const char *fn, FILE *fp, double start)
{
    bool worked = true;
    double value = start;
    for (int i = 0; i <= 125; ++i) {
        if (!read_check(fn, fp, value))
            worked = false;
        value *= 2;
    }
    value = start;
    for (int i = 0; i <= 125; ++i) {
        if (!read_check(fn, fp, value))
            worked = false;
        value /= 2;
    }
    return worked;
}

static int check_file(const char *fn)
{
    FILE *fp = fopen(fn, "rb");
    if (fp) {
        int status = 0;
        if (!read_check(fn, fp, 0.0))
            status = 1;
        if (!check_series(fn, fp, 1.0))
            status = 1;
        if (!check_series(fn, fp, -1.0))
            status = 1;
        return status;
    }
    else {
        fprintf(stderr, "test-fp: unable to open %s for reading: %s\n", fn, strerror(errno));
        return 1;
    }
}

static bool write_checked(double value, FILE *fp)
{
    if (!write_bbcfp(value, fp)) {
        fprintf(stderr, "test-fp: unable to write %g to file\n", value);
        return false;
    }
    return true;
}

static bool write_series(const char *fn, FILE *fp, double start)
{
    bool worked = true;
    double value = start;
    for (int i = 0; i <= 125; ++i) {
        if (!write_checked(value, fp))
            worked = false;
        value *= 2;
    }
    value = start;
    for (int i = 0; i <= 125; ++i) {
        if (!write_checked(value, fp))
            worked = false;
        value /= 2;
    }
    return worked;
}

static int write_file(const char *fn)
{
    FILE *fp = fopen(fn, "wb");
    if (fp) {
        int status = 0;
        if (!write_checked(0.0, fp))
            status = 1;
        if (!write_series(fn, fp, 1.0))
            status = 1;
        if (!write_series(fn, fp, -1.0))
            status = 1;
        fclose(fp);
        return status;
    }
    else {
        fprintf(stderr, "test-fp: unable to open %s for writing: %s\n", fn, strerror(errno));
        return 1;
    }
}

int main(int argc, char **argv)
{
    int status = 0;
    status += check_file("basdata");
    status += write_file("cdata");
    status += check_file("cdata");
    return status;
}
