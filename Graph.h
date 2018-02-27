#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma
#define WHITE 0
#define GREY 1
#define BLACK 2

int clock;

void touch(int* a, int x);

int get_ts(int* a, int x);

int* timestamparr;

char** files;

typedef struct graph_struct GRAPH;

extern GRAPH * graph_build(int n);

extern void graph_add_edge(GRAPH *g, int u, int v);


extern void graph_free(GRAPH * g);


extern void dfs(GRAPH *g, int src ,int* tp);


extern void dfs_r(GRAPH *g, int src, int *labels, int* tp);

extern void dfs_make(GRAPH* g, int src);

int dfs_rmake(GRAPH *g, int src, int labels);
