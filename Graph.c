#include "Graph.h"

int clock = 1;

void touch(int* a, int x) {
	a[x]= clock;
	//printf(" %s is now %d\n", files[x], clock);
	clock++;
	//printf("clock is now %d\n", clock);
}

int get_ts(int* a, int x) {
	return a[x];
}

int* timestamparr;

char** files;

typedef struct lst_node {
	int node_id;
	//int timestamp;
	struct lst_node *next;
} LST_NODE;

typedef struct {
	int out_degree;
	LST_NODE *neighbors;
} VERTEX;

struct graph_struct {
	int n; // num vertices

		   // array of vertices
	VERTEX *vertices;
};



GRAPH * graph_build(int n) {
	GRAPH * g = malloc(sizeof(GRAPH));
	g->n = n;
	g->vertices = malloc(n * sizeof(VERTEX));
	int i = 0;
	for (i = 0; i<n; i++) {
		g->vertices[i].out_degree = 0;
		g->vertices[i].neighbors = NULL;
	}
	return g;
}

void graph_add_edge(GRAPH *g, int u, int v)
{
	LST_NODE *new = malloc(sizeof(LST_NODE));
	new->node_id = v;
	//new->timestamp = 0;
	new->next = g->vertices[u].neighbors;
	g->vertices[u].neighbors = new;
	g->vertices[u].out_degree++;
}

void graph_free(GRAPH * g) {
	int i = 0;
	for (i = 0; i<g->n; i++) {
		LST_NODE *cur = g->vertices[i].neighbors;
		while (cur != NULL) {
			LST_NODE *temp = cur->next;
			free(cur);
			cur = temp;
		}
	}
	free(g->vertices);
	free(g);
}

void dfs(GRAPH *g, int src, int* tp) {
	int *labels =malloc(g->n * sizeof(int));
	int v = 0;

	for (v = 0; v<g->n; v++) {
		labels[v] = WHITE;
	}
	dfs_r(g, src, labels, tp);
	free(labels);
}

void dfs_r(GRAPH *g, int src, int *labels, int* tp) {
	LST_NODE *p;
	int v;
	labels[src] = GREY;
	p = g->vertices[src].neighbors;
	while (p != NULL) {
		v = p->node_id;
		if (labels[v] == WHITE)
			dfs_r(g, v, labels, tp);
		else if (labels[v] == GREY) {
			printf("cycle detected \n");
			(*tp)++;
		}
		p = p->next;
	}
	labels[src] = BLACK;
}

void dfs_make(GRAPH* g, int src) {

	int labels = timestamparr[src];
	int x=dfs_rmake(g, src, labels);
	if(x){ printf("making %s ........done \n", files[src]); }
}

int dfs_rmake(GRAPH *g, int src, int labels) {
	LST_NODE *p;
	int v;
	
	p = g->vertices[src].neighbors;
	//int srcTS = timestamparr[src];
	int OFDflag = 0;
	while (p != NULL) {
		v = p->node_id;
		if (timestamparr[v] > labels) { printf("making %s ........done \n",files[src]); OFDflag = 1; }
		else{ printf(" %s is up to date \n", files[v]); }
		OFDflag+=dfs_rmake(g, v, labels);

		p = p->next;
	}

	if (OFDflag) {
		//printf("incrementing file %s to %d\n",files[src],clock);
		touch(timestamparr, src);
	}
	return OFDflag;
}
