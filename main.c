#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK do { \
    if (ip < program || ip > &program[ncode])  \
        { printf("Bad Ip: %ld\n", ip - program); dump_state(); assert(0); } \
    if (ptr < data || ptr > &data[4])  \
        { printf("Bad Data Ptr: %ld\n", ptr - data); dump_state(); assert(0); } \
} while (0)

char data[30000] = { 0 };
char *ptr = data;
char program[30000] = { 0 };
char *ip = program;

void dump_state()
{
    int i;
    printf("[% 8ld] Data[0-9] ", ptr - data);
    for (i=0; i < 10; i++)
        printf("% 3d ", data[i]);

    printf("\n");
}

int
main(int argc, const char *argv[])
{
    int ncode = 0;

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

    ip = program;

    while (*ip != 0)
    {
        // in every moment, what is the right thing to do?

        //printf("%c", *ip);
        switch (*ip)
        {
        case '#': dump_state(); break;
        case '>':
            ++ptr;
            break;
        case '<':
            --ptr;
            break;
        case '+':
            ++*ptr;
            break;
        case '-':
            --*ptr;
            break;
        case '.':
            printf("%c", *ptr);
            break;
        case ',':
            *ptr = getchar();
            break;
        case '[':
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
                    CHECK;
                    if (*ip == '[') {
//                        printf(" (n=%d) ", n);
                        n++;
                    } else if (*ip == ']') {
//                        printf(" (n=%d) ", n);
                        n--;
                    }
                }
                while (n != 0 || *ip != ']');
                printf(" end ");
            }
            break;
        case ']':
            if (*ptr != 0)
            {
                // if the byte at the data pointer is nonzero, then instead of
                // moving the instruction pointer forward to the next command,
                // jump it back to the command after the matching [ command*.
                int n = 1;
                do {
                    ip--;

                    CHECK;
//                    printf("%c", *ip);

                    if (*ip == '[')
                    {
                        n--;
//                        printf(" (n=%d) ", n);
                    } else if (*ip == ']') {
                        n++;
//                        printf(" (n=%d) ", n);
                    }
                }
                while (n != 0 || *ip != '[');
//                printf(" loop ");
            }
            break;
        default:
            // ignore it
            break;
        };

        ip++;
        CHECK;
    }

    fclose(fp);

    return 0;
}
