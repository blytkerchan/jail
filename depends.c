#include "depends.h"
#include "edge.h"
#include "node.h"
#include "vector.h"
#include <assert.h>
#include <stdlib.h>
#include <libcontain/list.h>
#include <libcontain/vector.h>

/* HOW THIS WORKS
 * The dependency tracker creates an in-memory directed acyclic graph in 
 * which each node (or vertex, if you prefer) represents the information 
 * known for a given key, with its dependencies and other associated 
 * information.
 * The edges of the graph represent the dependencies and, as such, have 
 * information associated with them as well: most notably information about 
 * the action to satisfy the dependencies.
 * Nodes with no more dependencies to satisfy (except for dependencies that
 * block other nodes, ofcourse) are /leaf nodes/. Actions associated with
 * these nodes are presumably parallelizable. */

struct depends_type
{
	dep_key_cmp_func dep_key_cmp;
	list_t * nodes;
	vector_t * edges;
	vector_t * vectors;
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
	handle->edges = vector_new(0);
	if (!handle->edges)
		goto abort_dep_new2;
	handle->vectors = vector_new(0);
	if (!handle->vectors)
		goto abort_dep_new3;
	
	return handle;
	
	vector_free(handle->vectors);
	
abort_dep_new3:
	vector_free(handle->edges);
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
int dep_depends(depends_t * handle, uint32_t n_keys, const void ** keys)
{
	return -1;
}

/* set an inverse dependency: the currently selected key blocks the specified 
 * keys. To satisfy the dependency, VECTOR will be called with the currently 
 * selected key. */
int dep_blocks(depends_t * handle, uint32_t n_keys, const void ** keys)
{
	return -1;
}

struct dep_depends_on_helper_data_type
{
	int n_keys;
	const void ** keys;
	int satisfied;
};
typedef struct dep_depends_on_helper_data_type dep_depends_on_helper_data_t;

static void dep_depends_on_helper_helper(const void * v, void * d)
{
	dep_depends_on_helper_data_t * data = (dep_depends_on_helper_data_t*)d;
	depends_vector_t * vector = (depends_vector_t*)v;
	
	if (!(vector->flags & DEPENDS_VECTOR_FLAG_DONE))
		data->satisfied = 0;
}

static void dep_depends_on_helper(const void * e, void * d)
{
	dep_depends_on_helper_data_t * data = (dep_depends_on_helper_data_t*)d;
	depends_edge_t * edge = (depends_edge_t*)e;
	data->satisfied = 1;
	list_foreach(edge->vectors, dep_depends_on_helper_helper, data);
	if (!data->satisfied)
	{
		data->keys = realloc(data->keys, (data->n_keys + 2) * sizeof(void*));
		if (data->keys)
		{
			data->keys[data->n_keys++] = edge->nodes[0];
			data->keys[data->n_keys] = NULL;
		}
	}
}

/* get a NULL-terminated list of keys the current key depends on. Only 
 * unsatisfied dependencies will be listed and, for those, only the
 * keys - not the vectors nor the user-defined data. */
const void ** dep_depends_on(depends_t * handle, const void * key)
{
	dep_depends_on_helper_data_t data;
	data.keys = NULL;
	data.n_keys = 0;
	
	depends_node_t * node = depends_node_find(handle, key);
	if (node == NULL)
		return NULL;
	list_foreach(node->edges[1], dep_depends_on_helper, &data);
	
	return data.keys;
}

static void dep_blocking_on_helper(const void * e, void * d)
{
	dep_depends_on_helper_data_t * data = (dep_depends_on_helper_data_t*)d;
	depends_edge_t * edge = (depends_edge_t*)e;
	data->satisfied = 1;
	list_foreach(edge->vectors, dep_depends_on_helper_helper, data);
	if (!data->satisfied)
	{
		data->keys = realloc(data->keys, (data->n_keys + 1) * sizeof(void*));
		if (data->keys)
		{
			data->keys[data->n_keys++] = edge->nodes[1];
			data->keys[data->n_keys] = NULL;
		}
	}
}

/* get a NULL-terminated list of keys the current key still blocks. Only
 * unsatisfied dependencies will be listed and, for those, only the
 * keys - not the vectors nor the user-defined data */
const void ** dep_blocking(depends_t * handle, const void * key)
{
	dep_depends_on_helper_data_t data;
	data.keys = NULL;
	data.n_keys = 0;
	
	depends_node_t * node = depends_node_find(handle, key);
	if (node == NULL)
		return NULL;
	list_foreach(node->edges[0], dep_blocking_on_helper, &data);
	
	return data.keys;
}

struct dep_resolve_edge_visitor_data_type
{
	int edges_left;
	vector_t * satisfiable_edges;
};
typedef struct dep_resolve_edge_visitor_data_type dep_resolve_edge_visitor_data_t;

static void dep_resolve_retrieve_satisfiable_edge_helper(const void * e, void * d)
{
	const depends_edge_t * edge = (const depends_edge_t*)e;
	dep_resolve_edge_visitor_data_t * data = (dep_resolve_edge_visitor_data_t*)d;
	
	if (!edge)
		return;
	data->edges_left = 1;
	// is there a target node?
	if (edge->nodes[1])
	{
		// has the target node been satisfied
		if (edge->nodes[1]->flags & DEPENDS_NODE_SATISFIED)
		{
			// edge is not satisfiable
			return;
		}
	}
	// is there a source node?
	if (edge->nodes[0])
	{
		// has the source node been satisfied?
		if (!edge->nodes[0]->flags & DEPENDS_NODE_SATISFIED)
		{
			// does it have any incoming edges?
			if (!list_empty(edge->nodes[0]->edges[1]))
			{
				// edge is not satisfiable
				return;
			}
		}
	}
	// edge is satisfiable
	vector_push_back(data->satisfiable_edges, edge);
}

/* resolve all dependencies. This will call all the necessary vectors with 
 * the appropriate parameters to resolve all the entered dependencies. The 
 * OPTIONS field is reserved for now and should be 0. In some future version,
 * this will be used to fine-tune how the dependency tracker should work 
 * (probably in pair with a dep_ctl function to set some internal parameters
 * for the dependency tracker). */
int dep_resolve(depends_t * handle, uint32_t options)
{
	dep_resolve_edge_visitor_data_t data;
	data.satisfiable_edges = vector_new(0);
	if (data.satisfiable_edges == NULL)
		return -1;
	assert(options == 0);
	
	/* When we get here, we have a DAG in handle, but we have no idea as
	 * to which node - if any - has no unsatisfied dependencies. In fact,
	 * the DAG may be acyclic but still have no nodes without unsatisfied
	 * dependencies: some nodes may have dangling edges with their own 
	 * vectors attached to them. Hence, the only thing we can do is to 
	 * find any dangling edges and see if we can satisfy their vectors. */
	vector_t * edges = vector_copy(handle->edges);
	if (edges == NULL)
		goto abort_dep_resolve1;
	while (1)
	{
		/* clear the vector of satisfiable edges here */
		vector_clear(data.satisfiable_edges);
		vector_foreach(edges, dep_resolve_retrieve_satisfiable_edge_helper, &data);
		if (!data.edges_left)
			break;
		/* We now have a vector of satisfiable edges. For each of those edges, 
		 * get the vectors, see how many references to those vectors exist and
		 * whether they're all accounted for with the edges we have. There 
		 * should be at least one vector we can satisfy in every cycle of this loop. */
		/* Every edge that does not have any vectors left to be satisfied is, itself, 
		 * satisfied. Those edges should no longer be counted by the 
		 * dep_resolve_retrieve_satisfiable_edge_helper (a satisfied edge cannot 
		 * be satisfied again) */
	}
	
	vector_free(data.satisfiable_edges);
	vector_free(edges);
	return 0;

	vector_free(edges);
abort_dep_resolve1:
	vector_free(data.satisfiable_edges);
	return -1;
}

