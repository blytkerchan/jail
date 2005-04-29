#ifndef _libdepends_depends_h
#define _libdepends_depends_h

#include <stdint.h>

/* A simple but efficient dependency tracker.
 * This dependency tracker allows you to define dependencies between 
 * different keys, which must be comparable amongst eachother in a 
 * uniform way. The dependencies can be satified by calling vector
 * functions that take, as parameters, a number of keys and some 
 * user-defined data. The vector function shall return 0 if all is
 * well, or nonzero on error. The keys passed to the function will 
 * be the source keys listed for the dependency. The data passed 
 * to the vector is user-defined. */

struct depends_type;
typedef struct depends_type depends_t;
typedef int(*dep_key_cmp_func)(const void * key1, const void * key2);
typedef int(*dep_vector_func)(uint32_t n_keys, const void ** keys, void * data);

/* allocate a dependency tracker and its associated resources */
depends_t * dep_new(dep_key_cmp_func dep_key_cmp);
/* free a dependency tracker and its associated resources */
void dep_free(depends_t * handle);

/* add a key to the dependency tracker */
int dep_add(depends_t * handle, const void * key);

/* select a key to change its dependencies */
int dep_select(depends_t * handle, const void * key);
/* set a dependency. N_KEYS is the number of keys in KEYS; KEYS are the keys 
 * the currently selected key will depend on and VECTOR is the function to 
 * call to satisfy the dependency. */
int dep_depends(depends_t * handle, uint32_t n_keys, const void ** keys);
/* set an inverse dependency: the currently selected key blocks the specified 
 * keys. To satisfy the dependency, VECTOR will be called with the currently 
 * selected key. */
int dep_blocks(depends_t * handle, uint32_t n_keys, const void ** keys);
/* Note that both in the case of dep_depends and dep_blocks, the KEYS and 
 * VECTOR parameters may be NULL. If the VECTOR parameter is NULL, the 
 * dependency for the current key will be satisfied when the dependencies 
 * for all passed-in keys are satisfied. If the KEYS parameter is NULL, 
 * the dependency for the current key is satisfied upon successful 
 * completion of the VECTOR function. If both are NULL, the dependency is
 * automatically satisfied. */

/* get a NULL-terminated list of keys the current key depends on. Only 
 * unsatisfied dependencies will be listed and, for those, only the
 * keys - not the vectors nor the user-defined data. */
const void ** dep_depends_on(depends_t * handle, const void * key);
/* get a NULL-terminated list of keys the current key still blocks. Only
 * unsatisfied dependencies will be listed and, for those, only the
 * keys - not the vectors nor the user-defined data */
const void ** dep_blocking(depends_t * handle, const void * key);

/* set the action for the currently selected node */
int dep_action(depends_t * handle, dep_vector_func vector, void * data);

/* resolve all dependencies. This will call all the necessary vectors with 
 * the appropriate parameters to resolve all the entered dependencies. The 
 * OPTIONS field is reserved for now and should be 0. In some future version,
 * this will be used to fine-tune how the dependency tracker should work 
 * (probably in pair with a dep_ctl function to set some internal parameters
 * for the dependency tracker). */
int dep_resolve(depends_t * handle, uint32_t options);

#endif
