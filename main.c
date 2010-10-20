#include <assert.h>
#include <stdio.h>
#include <getopt.h>
#include <memory.h>

char code[30000];
char data[30000];
int dp = 0;
int ip = 0;
int ncode = 0;
int verbose = 0;
int extensions = 0;
FILE *fpinput = NULL;
FILE *fpgolden = NULL;

void dump_state()
{
    int i;
    fprintf(stderr, "IP=%d %c  DP=%d  ", ip, code[ip], dp);

    for (i=0; i < 10; i++)
        fprintf(stderr, "% 3d ", data[i]);

    fprintf(stderr, "\n");
}

void out_data()
{
    if (fpgolden == NULL || verbose)
        printf("%c", data[dp]);

    if (fpgolden)
    {
        char ch = fgetc(fpgolden);
        if (data[dp] != ch)
        {
            fprintf(stderr, "Unexpected output! '%c' != '%c'\n", data[dp], ch);
        } 
    }
}

static int scan(const char *p, int start, int dir, char match, char recurse)
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

int execute()
{
    bzero(data, sizeof(data));

    dp = 0;
    ip = 0;

    do { 
        if (ip < 0 || ip > ncode || dp < 0 || dp > sizeof(data))
        {
            fprintf(stderr, "out-of-bounds: ");
            dump_state(); 
            return -1;
        }

        if (verbose >= 3) dump_state();

        switch (code[ip])
        {
        case '>': dp += 1; break;
        case '<': dp -= 1; break;
        case '+': data[dp] += 1; break;
        case '-': data[dp] -= 1; break;
        case '[': skip_if_zero(); break;
        case ']': loop_if_nonzero(); break;
        case '.': out_data(); break;
        case ',': data[dp] = getc(fpinput); break;

        case '#': if (extensions && verbose) dump_state(); break;
        default:  if (verbose >= 2) fprintf(stderr, "%c", code[ip]); break;
        };

        ip++;

    } while (code[ip] != 0);
}

int main(int argc, char *argv[])
{
    int opt;
    char *fngolden = NULL;

    while ((opt = getopt(argc, argv, "vxt:")) != -1) 
    {
        switch (opt) {
        case 'v': ++verbose; break;
        case 'x': ++extensions; break;
        case 't': fngolden = optarg; break;
        };
    }

    if (optind == argc)
    {
        fprintf(stderr, "No input program\n");
        return 0;
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
       
        fpinput = stdin;
    
        if (fngolden)
        {
            fpgolden = fopen(fngolden, "r");
            if (fpgolden == NULL)
                perror(fngolden);
        }

        // load instructions into memory

        bzero(code, sizeof(code));
        ncode = 0;

        char ch;

        do {
            ch = fgetc(fp);

            if (extensions && ch == '!')
            {
                if (verbose) 
                    fprintf(stderr, "using input from '%s'\n", fn);

                fpinput = fp;
                break;
            } 

            code[ncode++] = ch;
        } while (ch != EOF);

        // execute instructions
        execute();

        // keep fp open during execution in case it was the input source
        fclose(fp);

        if (fpgolden)
        {
            // check for remaining expected output

            int n = 0;
            while ((ch = fgetc(fpgolden)) != EOF)
            {
                if (n++ == 0)
                    fprintf(stderr, "\nRemaining in '%s': ", fngolden);
                fputc(ch, stderr);
            }
            if (n > 0)
                fprintf(stderr, "\n%d more characters of output expected.\n", n);

            fclose(fpgolden);
        }

        // next file (will compare against same golden output
        ++optind;
    }

    return 0;
}
