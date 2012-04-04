#include <assert.h>
#include <stdio.h>
#include <getopt.h>
#include <memory.h>

char code[30000];
char data[30000];
int opcodes[256];
int dp = 0;
int ncode = 0;
int verbose = 0;
int extensions = 0;
FILE *fpinput = NULL;
FILE *fpgolden = NULL;

void dump_state(int ip)
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

int skip_if_zero(int ip)
{
    if (data[dp] == 0)
        ip = scan(code, ip, 1, ']', '[');

    return ip;
}

int loop_if_nonzero(int ip)
{
    if (data[dp] != 0) 
        ip = scan(code, ip, -1, '[', ']');

    return ip;
}

int compile(int ip)
{
    char op = code[ip+1];

    if (opcodes[op] != 0) {
        error("opcode '%c' previously defined", op);
        return;
    }

    opcodes[op] = ip+2;

    return scan(code, ip, 1, '}', '{');
}

int execute(int ip)
{
    while (1) { 
        if (ip < 0 || ip > ncode || dp < 0 || dp > sizeof(data))
        {
            fprintf(stderr, "out-of-bounds: ");
            dump_state(ip); 
            return -1;
        }

        char op = code[ip];

        if (verbose >= 3) dump_state(ip);

        switch (op)
        {
        case 0: return 0;  // program end
        case '>': dp += 1; break;
        case '<': dp -= 1; break;
        case '+': data[dp] += 1; break;
        case '-': data[dp] -= 1; break;
        case '[': ip = skip_if_zero(ip); break;
        case ']': ip = loop_if_nonzero(ip); break;
        case '.': out_data(); break;
        case ',': data[dp] = getc(fpinput); break;

        case '#': if (extensions && verbose) dump_state(ip); break;

        case '{': if (extensions) ip = compile(ip); break;
        case '}': if (extensions) return 1;  // procedure end

        default:  if (extensions) {
                      if (opcodes[op] != 0) {
                        int r = execute(opcodes[op]);
                        if (r <= 0) return r;
                      } 
                  } else if (verbose >= 2) fprintf(stderr, "%c", op);
                  break;
        };

        ip += 1;
    }
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
        const char *fn = argv[optind++];
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

        bzero(data, sizeof(data));
        bzero(opcodes, sizeof(opcodes));

        dp = 0;

        // execute starting at ip=0
        execute(0);

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

        // next file (will compare against same golden output)
    }

    return 0;
}
