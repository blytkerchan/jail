#include "depends.h"
#include <stdlib.h>
#include <libcontain/list.h>

/* HOW THIS WORKS
 * The dependency tracker creates an in-memory directed acyclic graph in 
 * which each node (or vertex, if you prefer) represents the information 
 * known for a given key, with its dependencies and other associated 
 * information.
 * The edges of the graph represent the dependencies and, as such, have 
 * information associated with them as well: most notably information about 
 * the action to satisfy the dependencies.
 * To help determine the order of the dependencies, each node in the graph 
 * has a score and an array of pointers to scores. The node with the highest
 * score in the graph has the most other nodes that depend on them.
 * Nodes with no more dependencies to satisfy (except for dependencies that
 * block other nodes, ofcourse) are /leaf nodes/. Actions associated with
 * these nodes are presumably parallelizable. */

struct depends_node_type;
typedef struct depends_node_type depends_node_t;
struct depends_edge_type;
typedef struct depends_edge_type depends_edge_t;
static int depends_edge_cmp(const void * k1, const void * k2);
struct depends_vector_type;
typedef struct depends_vector_type depends_vector_t;
static int depends_vector_cmp(const void * k1, const void * k2);

struct depends_node_type
{
	/* score of this node */
	uint32_t score;
	/* amount of pointers allocated in scores - not all of them need to be used.. */
	uint32_t n_scores;
	/* pointers to scores of nodes we depend on */
	uint32_t ** scores;
	/* the key this node represents */
	const void * key;
	/* the function used to compare keys */
	dep_key_cmp_func dep_key_cmp;
	/* edges that connect to this node. edges[0] are the outbound edges; edges[1] the inbound */
	list_t * edges[2];
};


static depends_node_t * depends_node_new(depends_t * handle, const void * key)
{
	depends_node_t * node = malloc(sizeof(depends_node_t));
	if (!node)
		return NULL;
	node->key = key;
	node->score = 1;
	node->n_scores = 0;
	node->scores = NULL;
	node->dep_key_cmp = handle->dep_key_cmp;
	node->edges[0] = list_new(depends_edge_cmp);
	if (!node->edges[0])
		goto abort_node_new1;
	node->edges[1] = list_new(depends_edge_cmp);
	if (node->edges[1])
		goto abort_node_new2;
	
	return node;
	
abort_node_new2:
	list_free(node->edges[0]);
abort_node_new1:
	free(node);
	return NULL;
}

static void depends_node_free(depends_node_t * node)
{
	list_free(node->edges[0]);
	list_free(node->edges[1]);
	free(node->scores);
	free(node);
}

static int depends_node_cmp(const void * k1, const void * k2)
{
	depends_node_t * node1 = (depends_node_t*)k1;
	depends_node_t * node2 = (depends_node_t*)k2;
	
	return node1->dep_key_cmp(node1->key, node2->key);
}

static depends_node_t * depends_node_find(depends_t * handle, const void * key)
{
	depends_node_t t_node;
	t_node.dep_key_cmp = handle->dep_key_cmp;
	t_node.key = key;
	return list_find(handle->nodes, t_node);
}

struct depends_edge_type
{
	/* the nodes this edge connects. nodes[0] is the node the edge source; nodes[1] the node it target. */
	depends_node_t * nodes[2];
	list_t * vectors;
};
typedef struct depends_edge_type depends_edge_t;

static depends_edge_t * depends_edge_new(depends_node_t * from, depends_node_t * to)
{
	depends_edge_t * edge = malloc(sizeof(depends_edge_t));
	if (!edge)
		return NULL;
	
	edge->nodes[0] = from;
	edge->nodes[1] = to;
	edge->vectors = list_new(depends_vector_cmp);
	if (!edge->vectors)
	{
		free(edge);
		return NULL;
	}
	
	return edge;
}

static void depends_edge_free(depends_edge_t * edge)
{
	free(edge);
}

static int depends_edge_cmp(const void * k1, const void * k2)
{
	depends_edge_t * edge1 = (depends_edge_t*)k1;
	depends_edge_t * edge2 = (depends_edge_t*)k2;
	
	if (edge1->nodes[0] != edge2->nodes[0])
		return edge1->nodes[0] - edge2->nodes[0];
	if (edge1->nodes[1] != edge2->nodes[1])
		return edge1->nodes[1] - edge2->nodes[1];
	
	return 0;
}

static depends_edge_t * depends_edge_find(depends_t * handle, depends_node_t * source, depends_node_t * target)
{
	depends_edge_t t_edge;
	t_edge.nodes[0] = source;
	t_edge.nodes[1] - target;
	
	return list_find(source->edges[0], &t_edge);
	
}

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
};

static depends_vector_t * depends_vector_new(dep_vector_func dep_vector, uint32_t n_keys, const void ** keys, void * data)
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

static void depends_vector_free(depends_vector_t * vector)
{
	free(vector);
}

struct depends_type
{
	dep_key_cmp_func dep_key_cmp;
	list_t * nodes;
	depends_node_t * curr;
};

/* allocate a dependency tracker and its associated resources */
depends_t * dep_new(dep_key_cmp_func dep_key_cmp)
{
	depends_t *  handle = malloc(sizeof(depends_t));
	if (!handle)
		return NULL;
	
	handle->dep_key_cmp = dep_key_cmp;
	handle->nodes = list_new(depends_node_cmp);
	if (!handle->nodes)
		goto abort_dep_new1;
	handle->curr = NULL;
	
	return handle;
	
abort_dep_new2:	
	list_free(handle->nodes);
abort_dep_new1:
	free(handle);
	return NULL;
}

/* free a dependency tracker and its associated resources */
void dep_free(depends_t * handle)
{
	list_free(handle->nodes);
	free(handle);
}

/* add a key to the dependency tracker */
int dep_add(depends_t * handle, const void * key)
{
	depends_node_t * node = depends_node_new(handle, key);
	if (!node)
		return -1;
	if (list_insert(handle->nodes, node) != 0)
	{
		depends_node_free(node);
		return -1;
	}
	
	return 0;
}

/* select a key to change its dependencies */
int dep_select(depends_t * handle, const void * key)
{
	handle->curr = depends_node_find(handle, key);
	if (!handle->curr)
		return -1;
	
	return 0;
}

/* set a dependency. N_KEYS is the number of keys in KEYS; KEYS are the keys 
 * the currently selected key will depend on and VECTOR is the function to 
 * call to satisfy the dependency. */
int dep_depends(depends_t * handle, uint32_t n_keys, const void ** keys, dep_vector_func vector, void * data)
{
	// create a vector for this dependency
	depends_vector_t * dep_vect = depends_vector_new(handle, n_keys, keys, vector, data);
	if (!dep_vect)
		return -1;
	
	// for each key in KEYS
	int i;
	for (i = 0; i < n_keys; i++)
	{
		depends_node_t * node = depends_node_find(keys[i]);
		if (!node)
			goto abort_depends;
		// find the corresponding edge in the current key
		depends_edge_t * edge = depends_edge_find(handle->curr, node);
		// does it already exist?
		if (!edge)
		{
			// create a new edge and add it to the list
			edge = depends_edge_new(handle, handle->curr, keys[i]);
			if (!edge)
				goto abort_depends;
		}
		// * add the new vector to the edge
		list_insert(edge->vectors, dep_vect);
	}
	// * update the scores of all nodes, checking for circular dependencies
	// | * for each key in KEYS
	// | | * get the corresponding node
	// | | ? see if a pointer to its score is already in the SCORES member 
	// | | - ? do we have space?
	// | | | - allocate it
	// | | * add the pointer to the SCORES member
	// | * propagate the score
	
abort_depends:
	// * tag the vector as invalid
	dep_vect->flag = DEPENDS_VECTOR_INVALID;
	return -1;
}

/* set an inverse dependency: the currently selected key blocks the specified 
 * keys. To satisfy the dependency, VECTOR will be called with the currently 
 * selected key. */
int dep_blocks(depends_t * handle, uint32_t n_keys, const void ** keys, dep_vector_func vector, void * data);
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

/* resolve all dependencies. This will call all the necessary vectors with 
 * the appropriate parameters to resolve all the entered dependencies. The 
 * OPTIONS field is reserved for now and should be 0. In some future version,
 * this will be used to fine-tune how the dependency tracker should work 
 * (probably in pair with a dep_ctl function to set some internal parameters
 * for the dependency tracker). */
int dep_resolve(depends_t * handle, uint32_t options);

#endif
