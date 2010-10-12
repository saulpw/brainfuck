#include <assert.h>
#include <stdio.h>
#include <getopt.h>
#include <memory.h>

char code[30000] = { 0 };
char data[30000] = { 0 };
int dp = 0;
int ip = 0;
int ncode = 0;
int verbose = 0;
int extensions = 0;
FILE *fpinput;
FILE *fpgolden = NULL;

void dump_state()
{
    int i;
    fprintf(stderr, "IP=%d %c  DP=%d  ", ip, code[ip], dp);

    for (i=0; i < 10; i++)
        fprintf(stderr, "% 3d ", data[i]);

    fprintf(stderr, "\n");
}

void advance_data(int n)
{
    dp += n;
}

void incr_data(int n)
{
    data[dp] += n;
}

void out_data()
{
    if (verbose || !fpgolden)
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

void in_data()
{
    data[dp] = getc(fpinput);
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
        case '>': advance_data(1); break;
        case '<': advance_data(-1); break;
        case '+': incr_data(1); break;
        case '-': incr_data(-1); break;
        case '.': out_data(); break;
        case ',': in_data(); break;
        case '[': skip_if_zero(); break;
        case ']': loop_if_nonzero(); break;

        case '#': if (extensions) dump_state(); break;
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

    if (fngolden)
    {
        fpgolden = fopen(fngolden, "r");
        if (fpgolden == NULL)
            perror(fngolden);
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

        fpinput = stdin;
    
        if (fp == NULL)
        {
            perror(fn);
            continue;
        }
       
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

        // assume non-null program
        execute();

        fclose(fp);

        if (fpgolden)
        {
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
        ++optind;
    }

    return 0;
}
