/*
 * Student Name: Gurleen Samra
 * Student ID:   
 * Date:         May 25
 * File:         sched.c
 * Description:  CPU scheduling simulator (FCFS and Round Robin).
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sched.h"


static void usage(const char* prog){
    fprintf(stderr,
        "Usage: %s --policy=FCFS|RR [--quantum=N] --in=FILE\n",
        prog);
}


int parse_args(int argc, char** argv, sim_cfg_t* cfg, const char** in_path){

    int policy_set = 0;
    cfg->quantum = -1;
    *in_path = NULL;

    for(int i = 1; i < argc; i++){

        if(strncmp(argv[i], "--policy=", 9) == 0){
            char* val = argv[i] + 9;

            if(strcmp(val, "FCFS") == 0){
                cfg->policy = POL_FCFS;
            }
            else if(strcmp(val, "RR") == 0){
                cfg->policy = POL_RR;
            }
            else{
                usage(argv[0]);
                return 1;
            }
            policy_set = 1;
        }

        else if(strncmp(argv[i], "--in=", 5) == 0){
            *in_path = argv[i] + 5;
        }

        else if(strncmp(argv[i], "--quantum=", 10) == 0){
            char* end;
            long q = strtol(argv[i] + 10, &end, 10);
            if(*end != '\0' || q <= 0){
                usage(argv[0]);
                return 1;
            }
            cfg->quantum = (int)q;
        }

        else{
            usage(argv[0]);
            return 1;
        }
    }

    if(!policy_set || *in_path == NULL){
        usage(argv[0]);
        return 1;
    }

    if(cfg->policy == POL_RR && cfg->quantum <= 0){
        usage(argv[0]);
        return 1;
    }

    if(cfg->policy == POL_FCFS && cfg->quantum > 0){
        usage(argv[0]);
        return 1;
    }

    return 0;
}


static int cmp(const void* a, const void* b){
    const job_t* x = a;
    const job_t* y = b;

    if(x->arrival != y->arrival)
        return x->arrival - y->arrival;

    return x->pid - y->pid;
}


int load_workload(const char* path, job_t** jobs, int* n){

    FILE* f = fopen(path, "r");
    if(f == NULL){
        return 1;
    }

    int cap = 8;
    *jobs = malloc(sizeof(job_t) * cap);
    *n = 0;

    job_t temp;

    while(1){
        int result = fscanf(f, "%d %d %d", &temp.pid, &temp.arrival, &temp.cpu_time);
        if(result != 3) break;

        if(temp.pid < 0 || temp.arrival < 0 || temp.cpu_time <= 0){
            fclose(f);
            free(*jobs);
            return 1;
        }

        // initialize scheduling fields 
        temp.remaining = temp.cpu_time;
        temp.first_run = -1;
        temp.completion = -1;

        // resize array if needed 
        if(*n == cap){
            cap = cap * 2;
            *jobs = realloc(*jobs, sizeof(job_t) * cap);
        }

        (*jobs)[*n] = temp;
        (*n)++;
    }

    fclose(f);

    qsort(*jobs, *n, sizeof(job_t), cmp);

    return 0;
}


int simulate(const job_t* input, int n, const sim_cfg_t* cfg, sim_metrics_t* out){

    job_t* p = malloc(sizeof(job_t) * n);
    memcpy(p, input, sizeof(job_t) * n);

    int timeline[10000];
    int tlen = 0;


    int total_burst = 0;
    for(int i = 0; i < n; i++) total_burst += p[i].cpu_time;
    int* q = malloc(sizeof(int) * (total_burst + n));
    int qhead = 0, qtail = 0;

    int cur = -1;
    int done = 0;
    int t = 0;
    int quantum = 0;
    int next_arrive = 0;

    while(done < n){

        // RR: re-enqueue preempted job before admitting new arrivals 
        if(cfg->policy == POL_RR && cur != -1 && quantum == 0){
            if(p[cur].remaining > 0){
                q[qtail++] = cur;
            }
            cur = -1;
        }

        // admit jobs that have arrived by tick t 
        while(next_arrive < n && p[next_arrive].arrival <= t){
            q[qtail++] = next_arrive;
            next_arrive++;
        }

        // pick next job from front of queue 
        if(cur == -1 && qhead < qtail){
            cur = q[qhead++];
            quantum = cfg->quantum;
            if(p[cur].first_run == -1){
                p[cur].first_run = t;
            }
        }

        // idle tick 
        if(cur == -1){
            timeline[tlen++] = -1;
            t++;
            continue;
        }

        // run one tick 
        timeline[tlen++] = p[cur].pid;
        p[cur].remaining--;
        quantum--;

        // process finished
        if(p[cur].remaining == 0){
            p[cur].completion = t + 1;
            cur = -1;
            done++;
        }

        t++;
    }

    // context switches 
    int ctx = 0;
    int prev = -1;

    for(int i = 0; i < tlen; i++){
        if(timeline[i] >= 0 && prev >= 0 && timeline[i] != prev) ctx++;
        if(timeline[i] >= 0) prev = timeline[i];
    }

    // metrics 
    double tat = 0, resp = 0;
    for(int i = 0; i < n; i++){
        tat  += p[i].completion - p[i].arrival;
        resp += p[i].first_run  - p[i].arrival;
    }

    // print timeline 
    printf("time:");
    for(int i = 0; i < tlen; i++) printf(" %d", i);
    printf("\nrun :");
    for(int i = 0; i < tlen; i++){
        if(timeline[i] == -1) printf(" -");
        else                  printf(" %d", timeline[i]);
    }
    printf("\n");

    // per-process stats 
    for(int i = 0; i < n; i++){
        printf("P%d: first run=%d completion=%d TAT=%d RESP=%d\n",
            p[i].pid,
            p[i].first_run,
            p[i].completion,
            p[i].completion - p[i].arrival,
            p[i].first_run  - p[i].arrival);
    }

    // system stats 
    printf("System: ctx_switches=%d, avgTAT=%.3f, avgRESP=%.3f\n",
        ctx, tat/n, resp/n);

    out->context_switches = ctx;
    out->avg_tat  = tat / n;
    out->avg_resp = resp / n;

    free(p);
    free(q);
    return 0;
}


int main(int argc, char** argv){

    sim_cfg_t cfg;
    const char* in_path = NULL;

    if(parse_args(argc, argv, &cfg, &in_path) != 0)
        return 1;

    job_t* jobs = NULL;
    int n = 0;

    if(load_workload(in_path, &jobs, &n) != 0)
        return 2;

    sim_metrics_t m;

    if(simulate(jobs, n, &cfg, &m) != 0){
        free(jobs);
        return 3;
    }

    free(jobs);
    return 0;
}