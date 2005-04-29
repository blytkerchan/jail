#include "depends.h"
#include "vector.h"
#include <assert.h>
#include <stdlib.h>
#include <libcontain/list.h>
#include <libcontain/vector.h>

#define DEPENDS_VECTOR_FLAG_INVALID	0x00000001
#define DEPENDS_VECTOR_FLAG_DONE	0x00000002

struct depends_vector_type
{
	/* the function to be run */
	dep_vector_func dep_vector;
	/* user data */
	void * data;
	/* number of keys to pass to the function */
	uint32_t n_keys;
	/* the keys to pass to the function */
	const void ** keys;
	int flags;
};

depends_vector_t * depends_vector_new(dep_vector_func dep_vector, uint32_t n_keys, const void ** keys, void * data)
{
	depends_vector_t * vector = malloc(sizeof(depends_vector_t));
	if (!vector)
		return NULL;
	
	vector->dep_vector = dep_vector;
	vector->n_keys = n_keys;
	vector->keys = keys;
	vector->data = data;
	
	return vector;
}

void depends_vector_free(depends_vector_t * vector)
{
	free(vector);
}

int depends_vector_cmp(const void * k1, const void * k2)
{
	int rv;

	rv = ((depends_vector_t*)k2)->dep_vector - ((depends_vector_t*)k1)->dep_vector;
	if (rv)
		return rv;
	rv = ((depends_vector_t*)k2)->n_keys - ((depends_vector_t*)k1)->n_keys;
	if (rv)
		return rv;
	rv = ((depends_vector_t*)k2)->keys - ((depends_vector_t*)k1)->keys;
	if (rv)
		return rv;
	rv = ((depends_vector_t*)k2)->data - ((depends_vector_t*)k1)->data;

	return rv;
}


