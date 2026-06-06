/* ID Header:
# Student Name: Omar Ghzeil, Gurleen Samra
# Student ID:3123197, 3144680
# Submission Date: June 7, 2026
 File: vmsim.c
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

/*
 * Purpose: Parses and validates command line arguments.
 * Parameters: argc, argv, o
 * Return: true on success, false on error
 */
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

    // loop through arguments
    for (int i = 1; i < argc; i++)
    {
        const char *arg = argv[i];

        // parse mode (bb or seg)
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
        // parse base value
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
        // parse limit value
        else if (strncmp(arg, "--limit=", 8) == 0)
        {
            char *end;

            long value = strtol(arg + 8, &end, 10);

            if (end == arg + 8 || *end != '\0')
            {
                fprintf(stderr, "Error: invalid limit value\n");
                return false;
            }

            o->limit = value;

            have_limit = true;
        }

        // trace file path
        else if (strncmp(arg, "--trace=", 8) == 0)
        {
            o->trace_path = arg + 8;
            have_trace = true;
        }
        // config file path
        else if (strncmp(arg, "--config=", 9) == 0)
        {
            o->config_path = arg + 9;
            have_config = true;
        }
        // unknown argument
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
    // base/bounds mode requirements
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

    // segment mode requirements
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


/*
 * Purpose: Runs the Base and Bounds simulation.
 * Parameters: o, st
 * Return: 0 on success, 1 on error
 */
int run_bb(const sim_opts_t *o, stats_t *st)
{
    if (o == NULL || st == NULL)
        return 1;

    // open trace file
    FILE *fp = fopen(o->trace_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: cannot open trace file %s\n", o->trace_path);
        return 1;
    }

    char line[256];
    int line_no = 0;
    // read trace line by line
    while (fgets(line, sizeof(line), fp))
    {
        line_no++;

         // remove newline
        char *p = strchr(line, '\n');
        if (p) *p = '\0';

        // remove comments
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

        // parse operation and address
        if (sscanf(line, "%10s %100s", op_str, addr_str) != 2)
        {
            printf("trace: %s:%d: malformed: expected \"OP ADDR\"\n", o->trace_path, line_no);
            continue;
        }

        // validate operation
        if (strcmp(op_str, "R") != 0 &&
            strcmp(op_str, "W") != 0)
        {
            printf("trace: %s:%d: malformed: op must be R/W, got \"%s\"\n", o->trace_path, line_no, op_str);
            continue;
        }

        char *end;

        // convert address
        long va = strtol(addr_str, &end, 10);

        if (end == addr_str || *end != '\0')
        {
            printf("trace: %s:%d: bad address \"%s\" (not decimal)\n",
                o->trace_path, line_no, addr_str);
            continue;
        }

        st->accesses++;

        // check bounds
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

    // print stats
    printf("== stats ==\n");
    printf("accesses=%lu, ok=%lu, faults.bounds=%lu\n", st->accesses, st->ok, st->faults_bounds);

    return 0;
}


/*
 * Purpose: Removes comments beginning with #
 * Parameters: line 
 * Return: None
 */
static void strip_comment_seg(char *line)
{
    char *comment = strchr(line, '#');

    if (comment != NULL)
        *comment = '\0';
}

/*
 * Purpose: Removes leading and trailing whitespace from a string
 * Parameters: line
 * Return: Pointer to trimmed string
 */
static char *trim_seg(char *line)
{
    while (isspace((unsigned char)*line))
        line++;

    if (*line == '\0')
        return line;

    char *end = line + strlen(line) - 1;

    while (end > line && isspace((unsigned char)*end))
    {
        *end = '\0';
        end--;
    }

    return line;
}

/*
 * Purpose: Converts a decimal string to a long integer
 * Parameters: text, out 
 * Return: true if conversion succeeds, false otherwise
 */
static bool parse_decimal_seg(const char *text, long *out)
{
    char *end;
    errno = 0;

    long value = strtol(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0')
        return false;

    *out = value;
    return true;
}

/*
 * Purpose: Validates a permissions string containing r, w, x.
 * Parameters: perms 
 * Return: true if valid, false otherwise
 */
static bool valid_perms(const char *perms)
{
    if (perms[0] == '\0' || strlen(perms) > 3)
        return false;

    for (size_t i = 0; perms[i] != '\0'; i++)
    {
        if (perms[i] != 'r' && perms[i] != 'w' && perms[i] != 'x')
            return false;
    }

    return true;
}

/*
 * Purpose: Loads segment definitions from a configuration file.
 * Parameters: path, table 
 * Return: 0 on success, 1 on error
 */
static int load_config(const char *path, seg_table_t *table)
{
    FILE *fp = fopen(path, "r");

    if (fp == NULL)
    {
        fprintf(stderr, "Error: cannot open config file %s\n", path);
        return 1;
    }

    table->nsegs = 0;

    char line[512];
    int line_no = 0;

    // read config line by line
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        line_no++;

        strip_comment_seg(line);
        char *clean = trim_seg(line);

        if (*clean == '\0')
            continue;

        char name[32];
        char base_str[100];
        char limit_str[100];
        char perms[10];
        char extra[100];

        // parse segment fields
        int parts = sscanf(clean, "%31s %99s %99s %9s %99s", name, base_str, limit_str, perms, extra);

        if (parts != 4)
        {
            printf("config: %s:%d: malformed: expected \"name base limit perms\"\n", path, line_no);
            continue;
        }

        long base;
        long limit;

        // validate numbers
        if (!parse_decimal_seg(base_str, &base) ||
            !parse_decimal_seg(limit_str, &limit))
        {
            printf("config: %s:%d: bad number in \"base/limit\"\n", path, line_no);
            continue;
        }

        if (limit < 0)
        {
            printf("config: %s:%d: bad number in \"base/limit\"\n", path, line_no);
            continue;
        }

        // check permissions
        if (!valid_perms(perms))
        {
            printf("config: %s:%d: malformed: invalid perms\n", path, line_no);
            continue;
        }

        // limit number of segments
        if (table->nsegs >= 16)
        {
            printf("config: %s:%d: malformed: too many segments\n", path, line_no);
            continue;
        }

        segment_t *seg = &table->segs[table->nsegs];

         // store segment
        strncpy(seg->name, name, sizeof(seg->name) - 1);
        seg->name[sizeof(seg->name) - 1] = '\0';

        seg->base = base;
        seg->limit = limit;

        strncpy(seg->perms, perms, sizeof(seg->perms) - 1);
        seg->perms[sizeof(seg->perms) - 1] = '\0';

        seg->in_use = true;
        seg->hits = 0;

        table->nsegs++;
    }

    fclose(fp);
    return 0;
}

/*
 * Purpose: Finds a segment by name in the segment table.
 * Parameters: table, name
 * Return: Pointer to matching segment or null if not found
 */
static segment_t *find_segment(seg_table_t *table, const char *name)
{
    for (size_t i = 0; i < table->nsegs; i++)
    {
        if (strcmp(table->segs[i].name, name) == 0)
            return &table->segs[i];
    }

    return NULL;
}

/*
 * Purpose: Determines the required permission for an operation
 * Parameters: op
  * Return: required permission character r,w,x
 */
static char needed_perm(char op)
{
    if (op == 'R')
        return 'r';

    if (op == 'W')
        return 'w';

    return 'x';
}

/*
 * Purpose: Checks if a segment allows a requested operation
 * Parameters: seg, op 
 * Return: true if permission exists, false otherwise
 */
static bool has_permission(const segment_t *seg, char op)
{
    char needed = needed_perm(op);

    return strchr(seg->perms, needed) != NULL;
}


/*
 * Purpose: Checks whether a segment allows a requested operation.
 * Parameters: seg, op 
 * Return: true if permission exists, false otherwise
 */
int run_seg(const sim_opts_t *o, stats_t *st)
{
    if (o == NULL || st == NULL)
        return 1;

    seg_table_t table;

    if (load_config(o->config_path, &table) != 0)
        return 1;

    FILE *fp = fopen(o->trace_path, "r");

    if (fp == NULL)
    {
        fprintf(stderr, "Error: cannot open trace file %s\n", o->trace_path);
        return 1;
    }

    char line[512];
    int line_no = 0;

    // read trace file
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        line_no++;

        strip_comment_seg(line);
        char *clean = trim_seg(line);

        if (*clean == '\0')
            continue;

        char op_str[10];
        char seg_name[32];
        char offset_str[100];
        char extra[100];

        // parse trace line
        int parts = sscanf(clean, "%9s %31s %99s %99s", op_str, seg_name, offset_str, extra);

        if (parts != 3)
        {
            printf("trace: %s:%d: malformed: expected \"OP SEG OFFSET\"\n", o->trace_path, line_no);
            continue;
        }

        if (strlen(op_str) != 1 ||
            (op_str[0] != 'R' && op_str[0] != 'W' && op_str[0] != 'X'))
        {
            printf("trace: %s:%d: malformed: op must be R/W/X, got \"%s\"\n", o->trace_path, line_no, op_str);
            continue;
        }

        char op = op_str[0];

        long offset;

        if (!parse_decimal_seg(offset_str, &offset))
        {
            printf("trace: %s:%d: bad offset \"%s\" (not decimal)\n", o->trace_path, line_no, offset_str);
            continue;
        }
        // check negative offset
        if (offset < 0)
        {
        printf("%s %s %ld -> malformed: expected \"OP SEG OFFSET\" (non-negative raw offset)\n", op_str, seg_name, offset);
            continue;
        }
        st->accesses++;

         // find segment
        segment_t *seg = find_segment(&table, seg_name);

        if (seg == NULL)
        {
            st->faults_noseg++;
            printf("%c %s %ld -> fault: NOSEG\n", op, seg_name, offset);
            continue;
        }

        bool in_bounds = false;
        long pa = 0;

        if (strcmp(seg->name, "stack") == 0)
        {
            long offsigned = offset - seg->limit;

            if (offsigned >= -seg->limit && offsigned < 0)
            {
                in_bounds = true;
                pa = seg->base + offsigned;
            }
        }
        else
        {
            if (offset >= 0 && offset < seg->limit)
            {
                in_bounds = true;
                pa = seg->base + offset;
            }
        }

        if (!in_bounds)
        {
            st->faults_bounds++;
            printf("%c %s %ld -> fault: BOUNDS\n", op, seg_name, offset);
            continue;
        }

        // check permissions
        if (!has_permission(seg, op))
        {
            st->faults_prot++;
            printf("%c %s %ld -> fault: PROTECTION (needed '%c', have '%s')\n", op, seg_name, offset, needed_perm(op), seg->perms);
            continue;
        }

        st->ok++;
        seg->hits++;

        printf("%c %s %ld -> PA %ld ; ok\n", op, seg_name, offset, pa);
    }

    fclose(fp);

    // final stats
    printf("== stats ==\n");
    printf("accesses=%lu, ok=%lu, faults.bounds=%lu\n", st->accesses, st->ok, st->faults_bounds);
    printf("faults.prot=%lu, faults.noseg=%lu\n", st->faults_prot, st->faults_noseg);

    printf("seg_hits:");

    for (size_t i = 0; i < table.nsegs; i++)
    {
        if (table.segs[i].hits > 0)
            printf(" %s=%lu", table.segs[i].name, table.segs[i].hits);
    }

    printf("\n");

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
