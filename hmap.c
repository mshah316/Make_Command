#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hmap.h"


static unsigned Coeff1 = DEFAULT_COEFF;


/******** STRUCTS AND TYPEDEFS *********/

typedef struct node_struct {
	struct node_struct *next;
	char *key;
	VAL_TYPE val;
	unsigned hval;
}NODE;

typedef struct {
	NODE *members;
	int n;
} TBL_ENTRY;

struct hmap {
	TBL_ENTRY *tbl;
	int tsize;
	int n;
	double lfactor;
	int max_n;
	HFUNC hfunc;
	char *hfunc_desc;
};

typedef struct {
	HFUNC hfunc;
	char *description;
} HFUNC_STRUCT;

/******** END STRUCTS AND TYPEDEFS *********/

/******** LIBRARY SUPPLIED HASH FUNCTIONS *****/

static unsigned h0(char *s) {
	unsigned h = 0;

	while (*s != '\0') {
		h += *s;
		s++;
	}
	return h;
}


static unsigned h1(char *s) {
	unsigned h = 0;

	while (*s != '\0') {
		h = h*Coeff1 + *s;
		s++;
	}
	return h;
}
/******** END LIBRARY SUPPLIED HASH FUNCTIONS *****/



/***** GLOBALS TO THIS FILE ******/

static HFUNC_STRUCT HashFunctions[] =
{
	{ h0, "naive char sum" },
	{ h1, "weighted char sum" }
};


static int NumHFuncs = sizeof(HashFunctions) / sizeof(HFUNC_STRUCT);

/***** END GLOBALS ******/

/***** FORWARD DECLARATIONS *****/
static int match(char *key, unsigned hval, NODE *p);
static NODE **get_node_pred(HMAP * map, char *key);
static TBL_ENTRY *create_tbl_array(int tsize);
static void resize(HMAP * map);
static void free_lst(NODE *l, int free_vals);
static void add_front(TBL_ENTRY *entry, NODE *p);
/***** END FORWARD DECLARATIONS *****/


/***** BEGIN hmap FUNCTIONS ******/

HMAP * hmap_create(unsigned init_tsize, double lfactor) {
	HMAP * map = malloc(sizeof(struct hmap));

	map->n = 0;
	if (lfactor <= 0)
		lfactor = DEFAULT_LFACTOR;
	if (init_tsize <= 0)
		init_tsize = DEFAULT_INIT_SIZE;

	map->lfactor = lfactor;
	map->tsize = init_tsize;
	map->max_n = init_tsize * lfactor;

	map->hfunc = HashFunctions[DEFAULT_HFUNC_ID].hfunc;
	map->hfunc_desc = HashFunctions[DEFAULT_HFUNC_ID].description;

	map->tbl = create_tbl_array(init_tsize);

	map->n = 0;

	return map;
}

void hmap_set_coeff(unsigned new_coeff) {

	Coeff1 = new_coeff;

}


int hmap_size(HMAP * map) {
	return map->n;
}

void hmap_display(HMAP * map) {
	int i, j;

	for (i = 0; i<map->tsize; i++) {
		printf("|-|");
		for (j = 0; j<map->tbl[i].n; j++)
			printf("X");
		printf("\n");
	}
}
int hmap_set_hfunc(HMAP * map, int hfunc_id) {
	if (map->n > 0) {
		fprintf(stderr,
			"warning:  attempt to change hash function on non-empty table\n");
		return 0;
	}
	if (hfunc_id < 0 || hfunc_id >= NumHFuncs) {
		fprintf(stderr,
			"warning:  invalid hash function id %i\n", hfunc_id);
		return 0;
	}
	map->hfunc = HashFunctions[hfunc_id].hfunc;
	map->hfunc_desc = HashFunctions[hfunc_id].description;
	return 1;
}

int hmap_set_user_hfunc(HMAP * map, HFUNC hfunc, char *desc) {
	if (map->n > 0) {
		fprintf(stderr,
			"warning:  attempt to change hash function on non-empty table\n");
		return 0;
	}
	map->hfunc = hfunc;
	if (desc == NULL)
		map->hfunc_desc = "user-supplied hash function";
	else
		map->hfunc_desc = desc;
	return 1;
}



int hmap_contains(HMAP * map, char *key) {
	NODE **pp;
	pp = get_node_pred(map, key);
	return (*pp == NULL ? 0 : 1);
}

VAL_TYPE hmap_get(HMAP * map, char *key) {
	NODE **pp;
	pp = get_node_pred(map, key);
	return (*pp == NULL ? DEFAULT_VAL : (*pp)->val);
}







VAL_TYPE hmap_set(HMAP * map, char *key, VAL_TYPE val) {
	unsigned h;
	int idx;
	NODE *p, **pp;

	pp = get_node_pred(map, key);
	p = *pp;

	if (p == NULL) {  // key not present
		char *key_clone;

		key_clone = malloc((strlen(key) + 1) * sizeof(char));
		strcpy(key_clone, key);

		map->n++;
		if (map->n > map->max_n)
			resize(map);
		h = map->hfunc(key);
		idx = h % map->tsize;

		p = malloc(sizeof(NODE));

		p->key = key_clone;
		p->val = val;
		p->hval = h;

		add_front(&(map->tbl[idx]), p);
		return DEFAULT_VAL;  // no old value to return
	}
	else {  // key already in map
		VAL_TYPE tmp = p->val;
		p->val = val;
		return tmp;  // return old value
	}
}

void hmap_insert(HMAP * map, char *key) {

	hmap_set(map, key, DEFAULT_VAL);

}

VAL_TYPE hmap_remove(HMAP * map, char *key) {
	NODE *p, **pp;

	pp = get_node_pred(map, key);
	p = *pp;
	if (p == NULL) {
		return DEFAULT_VAL;
	}
	else {
		unsigned idx;
		VAL_TYPE val = p->val;

		*pp = p->next;  // make predecessor skip node
						//   being removed
		free(p->key);
		free(p);

		idx = (p->hval) % map->tsize;
		map->tbl[idx].n--;
		map->n--;
		return val;
	}
}

static int num_empty(HMAP * map) {
	int i;
	int count = 0;

	for (i = 0; i<map->tsize; i++)
		if (map->tbl[i].n == 0)
			count++;

	return count;
}

static void print_max_len_bucket(HMAP * map) {
	int i, imax = 0;
	int max = 0;
	NODE *p;

	for (i = 0; i<map->tsize; i++) {
		if (map->tbl[i].n > max) {
			max = map->tbl[i].n;
			imax = i;
		}
	}

	p = map->tbl[imax].members;
	while (p != NULL) {
		printf("   '%s'\n", p->key);
		p = p->next;
	}

}

static int max_len(HMAP * map) {
	int i;
	int max = 0;

	for (i = 0; i<map->tsize; i++)
		if (map->tbl[i].n > max)
			max = map->tbl[i].n;

	return max;
}

static double avg_cmps(HMAP * map) {
	int i;
	double total = 0;

	for (i = 0; i<map->tsize; i++) {
		int ni = map->tbl[i].n;
		total += (ni*(ni + 1)) / 2;
	}
	return total / map->n;
}

static void length_distribution(HMAP *map) {
	int i;
	int freq[11] = { 0 };

	for (i = 0; i<map->tsize; i++) {
		int ni = map->tbl[i].n;
		if (ni < 11)
			freq[ni]++;
	}
	printf("BUCKET-LIST LENGTH PROFILE:\n\n");
	printf("  LIST-LENGTH       PERCENT  BUCKETS\n");
	for (i = 0; i<11; i++) {
		double percent;

		percent = ((double)freq[i]) / map->tsize;
		percent *= 100;

		printf("   %3i                   %2.3f\n", i, percent);
	}
	printf("--------------------------------\n");


}

void hmap_print_stats(HMAP * map) {

	// printf("sorry, hmap_print_stats() not implemented\n");

	printf("######## TABLE STATS ##########\n");

	printf("   hash-func:             %s \n", map->hfunc_desc);
	printf("   tblsize:               %i \n", map->tsize);
	printf("   numkeys:               %i \n", map->n);
	printf("   num empty slots:       %i \n", num_empty(map));
	printf("   max-collisions:        %i \n", max_len(map));
	printf("   avg-cmps-good-lookup:  %f \n", avg_cmps(map));

	printf("\n Keys In Biggest Bucket:\n");
	length_distribution(map);
	print_max_len_bucket(map);

	printf("###### END TABLE STATS ##########\n");

}

VAL_TYPE * hmap_extract_values(HMAP * map) {
	int i, k;
	NODE *p;
	VAL_TYPE *values = malloc(map->n * (sizeof(VAL_TYPE)));

	k = 0;
	for (i = 0; i<map->tsize; i++) {
		p = map->tbl[i].members;
		while (p != NULL) {
			values[k] = p->val;
			k++;
			p = p->next;
		}
	}
	return values;
}


KV_PAIR * hmap_extract_kv_pairs(HMAP * map) {
	int i, k;
	NODE *p;
	KV_PAIR *pairs = malloc(map->n * (sizeof(KV_PAIR)));

	k = 0;
	for (i = 0; i<map->tsize; i++) {
		p = map->tbl[i].members;
		while (p != NULL) {
			pairs[k].key = p->key;
			pairs[k].val = p->val;
			k++;
			p = p->next;
		}
	}
	return pairs;
}



void hmap_free(HMAP * map, int free_vals) {
	int i;

	for (i = 0; i<map->tsize; i++)
		free_lst(map->tbl[i].members, free_vals);
	free(map->tbl);
	map->tbl = NULL;  // not needed
	free(map);
}



/**** UTILITY FUNCTIONS *******/

static int match(char *key, unsigned hval, NODE *p) {
	return (p->hval == hval && strcmp(key, p->key) == 0);
}

static NODE **get_node_pred(HMAP * map, char *key) {
	unsigned h;
	int i;
	NODE **pp;
	h = map->hfunc(key);
	i = h % map->tsize;

	pp = &(map->tbl[i].members);
	while (*pp != NULL) {
		if (match(key, h, *pp))
			return pp;
		pp = &((*pp)->next);
	}
	return pp;
}

static void add_front(TBL_ENTRY *entry, NODE *p) {
	entry->n++;
	p->next = entry->members;
	entry->members = p;
}

static TBL_ENTRY *create_tbl_array(int tsize) {
	int i;
	TBL_ENTRY *tbl;
	NODE *p;

	tbl = malloc(tsize * sizeof(TBL_ENTRY));
	for (i = 0; i<tsize; i++) {
		tbl[i].members = NULL;
		tbl[i].n = 0;
	}
	return tbl;
}

static void resize(HMAP * map) {
	int ntsize;
	TBL_ENTRY *ntbl;
	NODE *nxt, *p;
	unsigned h;
	int i, idx;

	ntsize = 2 * map->tsize;
	ntbl = create_tbl_array(ntsize);

	for (i = 0; i<map->tsize; i++) {
		p = map->tbl[i].members;
		while (p != NULL) {
			nxt = p->next;
			// h = map->hfunc(key);  // no need to recompute
			idx = p->hval % ntsize;
			add_front(&ntbl[idx], p);
			p = nxt;
		}
	}
	free(map->tbl);
	map->tbl = ntbl;
	map->tsize = ntsize;
	map->max_n = (int)(ntsize * map->lfactor);

}
static void free_lst(NODE *l, int free_vals) {
	if (l == NULL) return;
	free_lst(l->next, free_vals);
	free(l->key);  // made our own copy
#ifdef POINTER_VAL
	if (free_vals &&  l->val != NULL)
		free(l->val);
#endif

	free(l);
}
/**** END UTILITY FUNCTIONS *******/


