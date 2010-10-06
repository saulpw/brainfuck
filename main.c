#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

char data[30000] = { 0 };
char *ptr = data;
char program[30000] = { 0 };
char *ip = program;
int ncode = 0;
int ndata = 0;
int debug = 0;
int verbose = 0;

void dump_state()
{
    if (!verbose) return;
    int i;
    if (debug) printf("\nIP=% 8ld  ", ip - program);
    printf("[% 8ld] Data[0-9] ", ptr - data);
    for (i=0; i < 10; i++)
        printf("% 3d ", data[i]);

    printf("\n");
}

void check_state()
{
    if (ip < program || ip > &program[ncode])
        { dump_state(); assert(0); }
    if (ptr < data || ptr > &data[4])
        { dump_state(); assert(0); }
}


void advance_data(int n)
{
    ptr += n;
}

void incr_data(int n)
{
    *ptr += n;
}

void out_comment()
{
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
// all problems are the same problem, at their core
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

int
main(int argc, const char *argv[])
{
    FILE *fp = fopen(argv[1], "r");
    
    if (fp == NULL)
    {
        perror(NULL);
        exit(1);
    }

    // first read in the entire program
    while (! feof(fp)) 
    {
        *ip = fgetc(fp);
        ip++;
        ncode++;
    }
    fclose(fp);

    ip = program;

    while (*ip != 0)
    {
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
    }

    return 0;
}
