#include <limits.h>
#include <float.h>
#include <stdio.h>

/**
* CONDITIONAL COMPILATION STUFF BELOW TO SET THE VALUE
* TYPE TO BE USED:
*
*   CHOICES:
*
*      TYPE         #define      gcc flag
*     double         DVAL         -DDVAL
*     int            IVAL         -DIVAL
*     char *         SVAL         -DSVAL
*     void *         PVAL         -DPVAL
*/

#ifndef _VAL_TYPE_SET
#ifdef DVAL
typedef double VAL_TYPE;
// char * ValDesc = "THIS HMAP COMPILED FOR double VALUES";
#define DEFAULT_VAL DBL_MAX
#define _VAL_TYPE_SET
#endif
#endif

#ifndef _VAL_TYPE_SET
#ifdef PVAL
#define POINTER_VAL
typedef void * VAL_TYPE;
// char * ValDesc = "THIS HMAP COMPILED FOR GENERIC void * VALUES";
#define DEFAULT_VAL NULL
#define _VAL_TYPE_SET
#endif
#endif

#ifndef _VAL_TYPE_SET
#ifdef SVAL
#define POINTER_VAL
typedef char * VAL_TYPE;
// char * ValDesc = "THIS HMAP COMPILED FOR STRING (char *) VALUES";
#define DEFAULT_VAL NULL
#define _VAL_TYPE_SET
#endif
#endif

#ifndef _VAL_TYPE_SET
#ifdef IVAL
typedef int VAL_TYPE;
// char * ValDesc = "THIS HMAP COMPILED FOR int VALUES";
#define DEFAULT_VAL INT_MAX
#define _VAL_TYPE_SET
#endif
#endif

#ifndef _VAL_TYPE_SET
typedef int VAL_TYPE;
// char * ValDesc = "THIS HMAP COMPILED FOR int VALUES BY DEFAULT";
#define DEFAULT_VAL INT_MAX
#define _VAL_TYPE_SET
#endif


// The KV_PAIR typedef is used for extracting the contents
//  of an hmap as a collection of KEY-VALUE pairs
typedef struct {
	char *key;
	VAL_TYPE val;
}KV_PAIR;

typedef unsigned(*HFUNC)(char *);

typedef struct hmap HMAP;

// default coefficient used for "basic weighted
//   hash function" (which is also default).
#define DEFAULT_COEFF 27

#define NAIVE_HFUNC 0
#define BASIC_WEIGHTED_HFUNC 1

#define DEFAULT_HFUNC_ID BASIC_WEIGHTED_HFUNC

#define DEFAULT_INIT_SIZE 128

#define DEFAULT_LFACTOR (0.75)

/**
* Creates and initializes a hash map data structure with
*   given initial table size and specified load factor.
*
* \param init_tsize specifies the desired initial
*    table size.  If zero is passed, a default table
*    size is used.
*
* \param lfactor specifies the desired maximum load factor;
*    if zero or a negative number is passed, a default
*    load factor is used.
* \returns HMAP * giving a handle to an initialized
*    empty hash map.
*/
extern HMAP * hmap_create(unsigned init_tsize, double lfactor);

/**
* sets coefficient for weighted hash function to given value.
* NOTE:  coefficient is set as a global and so affects all
*   hmaps in existence!
*
*   Call this function only when before any hmaps exist.
*/
void hmap_set_coeff(unsigned new_coeff);

/**
* \returns number of distinct keys in the map
*/
extern int hmap_size(HMAP * map);

/**
* prints a crude ascii profile of the distribution
*   of keys to table entries.
*   Shows collisions.
*/
extern void hmap_display(HMAP * map);

/**
* sets hash function as specified by hfunc_id to one
*   of the pre-defined hash functions.
* if table not empty, operation fails and returns 0.
* See #defines for NAIVE_HFUNC and BASIC_WEIGHTED_HFUNC
*   for example
*/
extern int hmap_set_hfunc(HMAP * map, int hfunc_id);

/**
* sets hash function to user-specified hfunc if table
*    empty (and returns 1).
* if table non-empty, hash function is unchanged and
*    zero is returned.
* \param desc is a string descibing the hash function.
*/
extern int hmap_set_user_hfunc(HMAP * map, HFUNC hfunc, char *desc);


/**
* determines if the given key is in the table and returns
*   0/1 accordingly.
* does not do anything with the value field.
*/
extern int hmap_contains(HMAP * map, char *key);


/**
* if given key is in table, corresponding value is returned.
* If not, NULL is returned.
*
* Note:  key could be in the table with NULL as its
*    corresponding value (as specified by the client).
*    In this case, the caller cannot distinguish between
*    a NULL value and the key not being in the table.
*
*    hmap_contains should be used in such cases
*/
extern VAL_TYPE hmap_get(HMAP * map, char *key);

/**
* sets the value associated with key to the given value
*   (val).
*
* If key not already in the table, a new entry is
*   created.  (There is no separate insert function)
* \returns previous value associated with key (if any);
*   DEFAULT_VAL if key not previously present.
*/
extern VAL_TYPE  hmap_set(HMAP * map, char *key, VAL_TYPE val);

/**
* inserts given key with associated value set to DEFAULT_VAL.
*   (val).
*
* if key already in table, the table is unchanged.
*
* Typical usage:  when hmap is used to represent a set -- i.e.,
*    application does not need associated values.
*
*/
extern void hmap_insert(HMAP * map, char *key);




/**
* Removes entry corresponding to given key (if any).
* \returns previously associated value (if key not
*    in table, NULL is returned.)
*/
extern VAL_TYPE hmap_remove(HMAP * map, char *key);


/**
* Prints statistical information about the map.
*
*/
extern void hmap_print_stats(HMAP * map);

/**
* Returns array of all n values in hmap (each value as a VAL_TYPE)
*
*/
extern VAL_TYPE * hmap_extract_values(HMAP * map);

/**
* Returns array of all key-value pairs in map
*
*/
extern KV_PAIR * hmap_extract_kv_pairs(HMAP * map);


/**
* Deallocates all memory internally allocated for the map
*
* If free_vals_flag is true, the associated values are also
*   deallocated (e.g., to save the client the trouble
*   of deallocating them).  If client did not dynamically
*   allocate values, then free_vals_flag should always be
*   false!
*
*/
extern void hmap_free(HMAP * map, int free_vals_flag);



