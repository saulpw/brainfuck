#include <assert.h>
#include <stdio.h>
#include <getopt.h>
#include <memory.h>

char code[30000] = { 0 };
char data[30000] = { 0 };
int dp = 0;
int ip = 0;
int ncode = 0;
int debug = 0;
int verbose = 0;

void dump_state()
{
    int i;
    printf("IP=%d %c  DP=%d  ", ip, code[ip], dp);
    for (i=0; i < 10; i++)
        printf("% 3d ", data[i]);

    printf("\n");
}

void check_state()
{
    if (ip < 0 || ip > ncode
     || dp < 0 || dp > sizeof(data))
    { 
        dump_state(); 
        assert(0); 
    }
}


void advance_data(int n)
{
    dp += n;
}

void incr_data(int n)
{
    data[dp] += n;
}

void out_comment()
{
    if (verbose)
        fprintf(stderr, "%c", code[ip]);
}

void out_data()
{
    printf("%c", data[dp]);
}

void in_data()
{
    data[dp] = getchar();
}

#ifdef RECURSIVE_SCAN
int scan(const char *p, int start, int dir, char match, char recurse)
{
    int idx = start;
    do {
        assert (idx >= 0);
        idx += dir;

        if (p[idx] == recurse)
            idx = scan(p, idx, dir, match, recurse) + dir;
    } while (p[idx] != match);

    return idx;
}
#else
int scan(const char *p, int start, int dir, char match, char recurse)
{
    int idx = start;
    int n = 1;
    do {
        assert (idx >= 0);
        idx += dir;

        if (p[idx] == recurse)    ++n;
        else if (p[idx] == match) --n;
    } while (n != 0);

    return idx;
}
#endif

void skip_if_zero()
{
    if (data[dp] == 0)
        ip = scan(code, ip, 1, ']', '[');
}

void loop_if_nonzero()
{
    if (data[dp] != 0) 
        ip = scan(code, ip, -1, '[', ']');
}

void execute()
{
    bzero(data, sizeof(data));

    dp = 0;
    ip = 0;

    do { 
        check_state();

        if (debug) dump_state();

        switch (code[ip])
        {
        case '>': advance_data(1); break;
        case '<': advance_data(-1); break;
        case '+': incr_data(1); break;
        case '-': incr_data(-1); break;
        case '.': out_data(); break;
        case ',': in_data(); break;
        case '[': skip_if_zero(); break;
        case ']': loop_if_nonzero(); break;
        case '#': dump_state(); break;
        default:  out_comment(); break;
        };

        ip++;

    } while (code[ip] != 0);
}

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "dv")) != -1) 
    {
        switch (opt) {
            case 'd': ++debug; break;
            case 'v': ++verbose; break;
        };
    }

    while (optind < argc)
    {
        const char *fn = argv[optind];
        FILE *fp = fopen(fn, "r");
    
        if (fp == NULL)
        {
            perror(fn);
            continue;
        }
       
        bzero(code, sizeof(code));

        ncode = 0;

        while (! feof(fp)) 
        {
            code[ncode++] = fgetc(fp);
        }

        fclose(fp);

        // assume non-null program
        execute();

        ++optind;
    }

    return 0;
}
