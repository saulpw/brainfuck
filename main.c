#include <assert.h>
#include <stdio.h>
#include <getopt.h>
#include <memory.h>

char program[60000] = { 0 };
char *data;
char *ptr;
char *ip;
int ncode;
int ndata;
int debug = 0;
int verbose = 0;

void dump_state()
{
    int i;
    printf("IP=%ld/%d DP=%ld/%d  ", ip - program, ncode, ptr - data, ndata);
    for (i=0; i < 10; i++)
        printf("% 3d ", data[i]);

    printf("\n");
}

void check_state()
{
    if (ip < program || ip > &program[ncode]
     || ptr < data || ptr > &data[ndata]
     || ndata >= sizeof(program) - ncode)
    { 
        dump_state(); 
        assert(0); 
    }
}


void advance_data(int n)
{
    ptr += n;
    if (ptr - data > ndata) {
        ndata = ptr - data;
        if (debug) { printf("[first use]"); dump_state(); }
    }
}

void incr_data(int n)
{
    *ptr += n;
}

void out_comment()
{
    if (verbose)
        fprintf(stderr, "%c", *ip);
}

void out_data()
{
    printf("%c", *ptr);
}

void in_data()
{
    *ptr = getchar();
}

void comment()
{
    do {
        ip++;
        if (*ip == '(') 
            comment();
    } while (*ip != ')');
    ip++;
}

// dir = 1 or -1
int find(const char *p, int dir, char c, char recurse)
{
    int n = 0;
    do {
        n += dir;
        if (p[n] == recurse)
            find(p + n, dir, c, recurse);
    } while (p[n] != c);

    return n * dir;
}

void scan_ip(int dirlevel, char recurse, char match)
{
    int dir = dirlevel > 0 ? 1 : -1;
    do {
        ip += dir;
        check_state();

        if (*ip == recurse)
            scan_ip(dirlevel + dir, recurse, match);
    } while (*ip != match);
}

#define EXTRA

void execute()
{
    data = &program[ncode];

    ptr = data;
    ip = program;
    ndata = 0;

    do { 
        if (debug && verbose) printf("%c", *ip);

        switch (*ip)
        {
        case '>': advance_data(1); break;
        case '<': advance_data(-1); break;
        case '+': incr_data(1); break;
        case '-': incr_data(-1); break;
        case '.': out_data(); break;
        case ',': in_data(); break;
        case '[': if (*ptr == 0) { ip += find(ip, 1, ']', '['); } break;
        case ']': if (*ptr != 0) { ip += find(ip, -1, '[', ']'); } break;
#ifdef EXTRA
        case '#': dump_state(); break;
        case '(': comment(); break;
        case '1': 
        case '2': 
        case '3': 
        case '4': 
        case '5': 
        case '6': 
        case '7': 
        case '8': 
        case '9': *ptr += *ip - '0'; break;
#endif
        default:  out_comment(); break;
        };

        ip++;
        check_state();
    } while (*ip != 0);
}

int
main(int argc, char *argv[])
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
       
        bzero(program, sizeof(program));
        ncode = 0;

        while (! feof(fp)) 
        {
            program[ncode++] = fgetc(fp);
        }

        fclose(fp);

        // assume non-null program
        execute();

        ++optind;
    }

    return 0;
}
