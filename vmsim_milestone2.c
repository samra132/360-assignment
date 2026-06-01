/* ID Header:
 Student Name:
 Student ID:
 Submission Date:
 File:
*/

#include "vmsim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// usage
static void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s --mode=bb  --base=N --limit=N --trace=FILE \n"
        "  %s --mode=seg --config=FILE --trace=FILE \n",
        prog, prog);
}

// CLI
bool parse_args(int argc, char **argv, sim_opts_t *o)
{
    if (o == NULL)
        return false;

    memset(o, 0, sizeof(*o));

    bool have_mode = false;
    bool have_trace = false;
    bool have_base = false;
    bool have_limit = false;
    bool have_config = false;

    for (int i = 1; i < argc; i++)
    {
        const char *arg = argv[i];

        if (strncmp(arg, "--mode=", 7) == 0)
        {
            const char *mode = arg + 7;

            if (strcmp(mode, "bb") == 0)
                o->mode = MODE_BB;
            else if (strcmp(mode, "seg") == 0)
                o->mode = MODE_SEG;
            else
            {
                fprintf(stderr, "Error: mode must be bb or seg\n");
                return false;
            }

            have_mode = true;
        }

        else if (strncmp(arg, "--base=", 7) == 0)
        {
            char *end;

            long value = strtol(arg + 7, &end, 10);

            if (end == arg + 7 || *end != '\0')

            {

                fprintf(stderr, "Error: invalid base value\n");

                return false;

            }

            o->base = value;

            have_base = true;
        }

        else if (strncmp(arg, "--limit=", 8) == 0)
        {
            o->limit = strtol(arg + 8, NULL, 10);
            have_limit = true;
        }

        else if (strncmp(arg, "--trace=", 8) == 0)
        {
            o->trace_path = arg + 8;
            have_trace = true;
        }

        else if (strncmp(arg, "--config=", 9) == 0)
        {
            o->config_path = arg + 9;
            have_config = true;
        }

        else
        {
            fprintf(stderr, "Error: unknown option %s\n", arg);
            return false;
        }
    }


    if (have_mode == false)
    {
        fprintf(stderr, "Error: missing --mode\n");
        return false;
    }

    if (have_trace == false)
    {
        fprintf(stderr, "Error: missing --trace\n");
        return false;
    }

    if (o->mode == MODE_BB)
    {
        if (have_base == false)
        {
            fprintf(stderr, "Error: missing --base\n");
            return false;
        }

        if (have_limit == false)
        {
            fprintf(stderr, "Error: missing --limit\n");
            return false;
        }
    }

    if (o->mode == MODE_SEG)
    {
        if (have_config == false)
        {
            fprintf(stderr, "Error: missing --config\n");
            return false;
        }
    }

    return true;
}


//bb
int run_bb(const sim_opts_t *o, stats_t *st)
{
    if (o == NULL || st == NULL)
        return 1;

    FILE *fp = fopen(o->trace_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: cannot open trace file %s\n", o->trace_path);
        return 1;
    }

    char line[256];
    int line_no = 0;

    while (fgets(line, sizeof(line), fp))
    {
        line_no++;

        char *p = strchr(line, '\n');
        if (p) *p = '\0';

        char *comment = strchr(line, '#');
        if (comment)
            *comment = '\0';

        int len = strlen(line);
        while (len > 0 && isspace((unsigned char)line[len - 1]))
        {
            line[len - 1] = '\0';
            len--;
        }

        if (len == 0)
            continue;

        char op_str[10];
        char addr_str[100];

        if (sscanf(line, "%10s %100s", op_str, addr_str) != 2)
        {
            printf("trace: %s:%d: malformed: expected \"OP ADDR\"\n", o->trace_path, line_no);
            continue;
        }

        if (strcmp(op_str, "R") != 0 &&
            strcmp(op_str, "W") != 0)
        {
            printf("trace: %s:%d: malformed: op must be R/W, got \"%s\"\n", o->trace_path, line_no, op_str);
            continue;
        }

        char *end;

        long va = strtol(addr_str, &end, 10);

        if (end == addr_str || *end != '\0')
        {
            printf("trace: %s:%d: bad address \"%s\" (not decimal)\n",
                o->trace_path, line_no, addr_str);
            continue;
        }

        st->accesses++;

        if (va >= 0 && va < o->limit)
        {
            long pa = o->base + va;

            printf("%s %ld -> PA %ld ; ok\n", op_str, va, pa);

            st->ok++;
        }
        else
        {
            printf("%s %ld -> fault: BOUNDS\n", op_str, va);

            st->faults_bounds++;
        }
    }

    fclose(fp);

    printf("== stats ==\n");
    printf("accesses=%lu, ok=%lu, faults.bounds=%lu\n", st->accesses, st->ok, st->faults_bounds);

    return 0;
}

//seg
int run_seg(const sim_opts_t *o, stats_t *st)
{
    (void)o;
    (void)st;
    return 0;
}

//main()
int main(int argc, char **argv) {
    sim_opts_t opts;
    if (!parse_args(argc, argv, &opts)) { usage(argv[0]); return 1; }
    stats_t st = (stats_t){0};
    if (opts.mode == MODE_BB) return run_bb(&opts, &st);
    else return run_seg(&opts, &st);
}
