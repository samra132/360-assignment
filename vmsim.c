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

    // validation 
    
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

void print_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        fprintf(stderr, "Could not open %s\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp))
    {
        //remove newline
        line[strcspn(line, "\n")] = '\0';

        //remove comments
        char *comment = strchr(line, '#');
        if (comment)
            *comment = '\0';

        
        int len = strlen(line);
        while (len > 0 && isspace((unsigned char)line[len - 1]))
        {
            line[len - 1] = '\0';
            len--;
        }

        // skip empty lines
        if (len == 0)
            continue;

        printf("%s\n", line);
        
    }
    
}

//bb
int run_bb(const sim_opts_t *o, stats_t *st)
{
    (void)st;

    print_file(o->trace_path);

    return 0;
}

//seg
int run_seg(const sim_opts_t *o, stats_t *st)
{
    (void)st;

    print_file(o->config_path);
    print_file(o->trace_path);

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
