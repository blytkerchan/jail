#ifndef _libdepends_vector_h
#define _libdepends_vector_h

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
typedef struct depends_vector_type depends_vector_t;

depends_vector_t * depends_vector_new(dep_vector_func dep_vector, uint32_t n_keys, const void ** keys, void * data);
void depends_vector_free(depends_vector_t * vector);
int depends_vector_cmp(const void * k1, const void * k2);

#endif

