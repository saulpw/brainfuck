#include <assert.h>
#include <stdio.h>
#include <getopt.h>
#include <memory.h>

char data[30000];
char *ptr;
char program[30000] = { 0 };
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
     || ndata >= sizeof(data))
    { 
        dump_state(); 
        assert(0); 
    }
}


void advance_data(int n)
{
    ptr += n;
    if (ptr - data > ndata) ndata = ptr - data;
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

void begin_loop()
{
    if (*ptr == 0)
    {
        // if the byte at the data pointer is zero, then instead of
        // moving the instruction pointer forward to the next command,
        // jump it forward to the command after the matching ]
        // command*.
        int n = 1;
        do {
            ip++;
            check_state();
            if (*ip == '[') {
                n++;
            } else if (*ip == ']') {
                n--;
            }
        }
        while (n != 0 || *ip != ']');
    }
}

void end_loop()
{
    if (*ptr != 0)
    {
        // if the byte at the data pointer is nonzero, then instead of
        // moving the instruction pointer forward to the next command,
        // jump it back to the command after the matching [ command*.
        int n = 1;
        do {
            ip--;

            check_state();
            if (debug) printf("%c", *ip);

            if (*ip == '[')
            {
                n--;
            } else if (*ip == ']') {
                n++;
            }
        }
        while (n != 0 || *ip != '[');
    }
}

void execute()
{
    bzero(data, sizeof(data));
    ptr = data;
    ip = program;
    ndata = 0;

    do { 
        if (debug) printf("%c", *ip);

        switch (*ip)
        {
        case '#': dump_state(); break;
        case '>': advance_data(1); break;
        case '<': advance_data(-1); break;
        case '+': incr_data(1); break;
        case '-': incr_data(-1); break;
        case '.': out_data(); break;
        case ',': in_data(); break;
        case '[': begin_loop(); break;
        case ']': end_loop(); break;
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
       
        ncode = 0;
        // first read in the entire program
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
