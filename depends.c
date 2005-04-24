#include "depends.h"
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

#define DEPENDS_NODE_FLAG_VISITED	0x00000001

#define DEPENDS_VECTOR_FLAG_INVALID	0x00000001
#define DEPENDS_VECTOR_FLAG_DONE	0x00000002

struct depends_node_type;
typedef struct depends_node_type depends_node_t;
struct depends_edge_type;
typedef struct depends_edge_type depends_edge_t;
static int depends_edge_cmp(const void * k1, const void * k2);
struct depends_vector_type;
typedef struct depends_vector_type depends_vector_t;
static int depends_vector_cmp(const void * k1, const void * k2);

typedef void (*depends_node_visitor_func)(depends_node_t *, void * data);

struct depends_node_type
{
	/* the key this node represents */
	const void * key;
	/* the function used to compare keys */
	dep_key_cmp_func dep_key_cmp;
	/* edges that connect to this node. edges[0] are the outbound edges; edges[1] the inbound */
	list_t * edges[2];
	int flags;
};

struct depends_edge_type
{
	/* the nodes this edge connects. nodes[0] is the node the edge source; nodes[1] the node it target. */
	depends_node_t * nodes[2];
	list_t * vectors;
	int invalid;
};

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

struct depends_type
{
	dep_key_cmp_func dep_key_cmp;
	list_t * nodes;
	vector_t * edges;
	vector_t * vectors;
	depends_node_t * curr;
};

struct depends_node_visitor_helper_data_type
{
	int fail;
};
typedef struct depends_node_visitor_helper_data_type depends_node_visitor_helper_data_t;

static void depends_node_visit_helper(const void * edge, void * data);
static int depends_node_visit(depends_node_t * node, depends_node_visitor_func visitor, void * d)
{
	depends_node_visitor_helper_data_t data;
	data.fail = 0;
	
	if (node->flags & DEPENDS_NODE_FLAG_VISITED)
		return -1;
	if (visitor)
		visitor(node, d);
	node->flags |= DEPENDS_NODE_FLAG_VISITED;
	list_foreach(node->edges[0], depends_node_visit_helper, &data);
	node->flags ^= DEPENDS_NODE_FLAG_VISITED;

	return data.fail ? -1 : 0;
}

static depends_node_t * depends_node_new(depends_t * handle, const void * key)
{
	depends_node_t * node = malloc(sizeof(depends_node_t));
	if (!node)
		return NULL;
	node->key = key;
	node->dep_key_cmp = handle->dep_key_cmp;
	node->edges[0] = list_new(depends_edge_cmp);
	if (!node->edges[0])
		goto abort_node_new1;
	node->edges[1] = list_new(depends_edge_cmp);
	if (node->edges[1])
		goto abort_node_new2;
	node->flags = 0;
	
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
	return list_search(handle->nodes, &t_node);
}

static void depends_node_visit_helper(const void * edge, void * data)
{
	if (((depends_node_visitor_helper_data_t*)data)->fail == 0 && ((depends_edge_t*)edge)->nodes[1])
	{
		if (depends_node_visit(((depends_edge_t*)edge)->nodes[1], NULL, NULL) != 0)
			((depends_node_visitor_helper_data_t*)data)->fail = 0;
	}
}

static depends_edge_t * depends_edge_new(depends_t * handle, depends_node_t * from, depends_node_t * to)
{
	depends_edge_t * edge = malloc(sizeof(depends_edge_t));
	if (!edge)
		return NULL;
	
	edge->nodes[0] = from;
	edge->nodes[1] = to;
	edge->vectors = list_new(depends_vector_cmp);
	edge->invalid = 0;
	if (!edge->vectors)
		goto abort_depends_edge_new1;
	if (vector_push_back(handle->edges, edge) != 0)
		goto abort_depends_edge_new2;
	
	return edge;

abort_depends_edge_new2:
	list_free(edge->vectors);
abort_depends_edge_new1:
	free(edge);
	return NULL;
}

static void depends_edge_free(depends_edge_t * edge)
{
	list_free(edge->vectors);
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
	t_edge.nodes[1] = target;
	
	return list_search(source->edges[0], &t_edge);
	
}

static int depends_edge_insert(depends_t * handle, depends_edge_t * edge)
{
	/* to be able to insert an edge in the graph, there must be no path
	 * from the target to the source node so we recursively traverse 
	 * the graph to see if we find any circular depedencies. */
	// * tag the source node as visited
	edge->nodes[0]->flags |= DEPENDS_NODE_FLAG_VISITED;
	// * visit the target node
	int rv = depends_node_visit(edge->nodes[1], NULL, NULL);
	// * untag the source node
	edge->nodes[0]->flags ^= DEPENDS_NODE_FLAG_VISITED;

	return rv;
}

static void depends_edge_invalidate(const void * ptr, void * p2)
{
	depends_edge_t * edge = (depends_edge_t*)ptr;
	edge->invalid = 1;
}

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

static int depends_vector_cmp(const void * k1, const void * k2)
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
int dep_depends(depends_t * handle, uint32_t n_keys, const void ** keys, dep_vector_func vector, void * data)
{
	/* Note that this vector will only contain the newly created edges:
	 * the edges we already had are valid so we don't have to invalidate
	 * them */
	vector_t * edges = vector_new(n_keys);
	
	// create a vector for this dependency
	depends_vector_t * dep_vect = depends_vector_new(vector, n_keys, keys, data);
	if (!dep_vect)
		return -1;
	
	// for each key in KEYS
	if (n_keys)
	{
		int i;
		for (i = 0; i < n_keys; i++)
		{
			depends_node_t * node = depends_node_find(handle, keys[i]);
			if (!node)
				goto abort_depends;
			// find the corresponding edge in the current key
			depends_edge_t * edge = depends_edge_find(handle, handle->curr, node);
			// does it already exist?
			if (!edge)
			{
				// create a new edge and add it to the list
				edge = depends_edge_new(handle, handle->curr, node);
				if (!edge)
					goto abort_depends;
				vector_push_back(edges, edge);
				depends_edge_insert(handle, edge);
			}
			if (edge->invalid)
				goto abort_depends;
	
			// * add the new vector to the edge
			list_insert(edge->vectors, dep_vect);
		}
	}
	else
	{
		depends_edge_t * edge = depends_edge_find(handle, handle->curr, NULL);
		if (!edge)
		{
			edge = depends_edge_new(handle, handle->curr, NULL);
			if (!edge)
				goto abort_depends;
			vector_push_back(edges, edge);
			depends_edge_insert(handle, edge);
		}
		if (edge->invalid)
			goto abort_depends;

		list_insert(edge->vectors, dep_vect);
	}
	
	vector_free(edges);
	return 0;
abort_depends:
	// * tag the vector as invalid
	dep_vect->flags |= DEPENDS_VECTOR_FLAG_INVALID;
	vector_foreach(edges, depends_edge_invalidate, NULL);
	vector_free(edges);
	return -1;
}

/* set an inverse dependency: the currently selected key blocks the specified 
 * keys. To satisfy the dependency, VECTOR will be called with the currently 
 * selected key. */
int dep_blocks(depends_t * handle, uint32_t n_keys, const void ** keys, dep_vector_func vector, void * data)
{
	/* Note that this vector will only contain the newly created edges:
	 * the edges we already had are valid so we don't have to invalidate
	 * them */
	vector_t * edges = vector_new(n_keys);
	
	// create a vector for this dependency
	depends_vector_t * dep_vect = depends_vector_new(vector, n_keys, keys, data);
	if (!dep_vect)
		return -1;
	
	// for each key in KEYS
	if (n_keys)
	{
		int i;
		for (i = 0; i < n_keys; i++)
		{
			depends_node_t * node = depends_node_find(handle, keys[i]);
			if (!node)
				goto abort_depends;
			// find the corresponding edge in the current key
			depends_edge_t * edge = depends_edge_find(handle, node, handle->curr);
			// does it already exist?
			if (!edge)
			{
				// create a new edge and add it to the list
				edge = depends_edge_new(handle, node, handle->curr);
				if (!edge)
					goto abort_depends;
				vector_push_back(edges, edge);
				depends_edge_insert(handle, edge);
			}
			if (edge->invalid)
				goto abort_depends;
	
			// * add the new vector to the edge
			list_insert(edge->vectors, dep_vect);
		}
	}
	else
	{
		depends_edge_t * edge = depends_edge_find(handle, NULL, handle->curr);
		if (!edge)
		{
			edge = depends_edge_new(handle, NULL, handle->curr);
			if (!edge)
				goto abort_depends;
			vector_push_back(edges, edge);
			depends_edge_insert(handle, edge);
		}
		if (edge->invalid)
			goto abort_depends;

		list_insert(edge->vectors, dep_vect);
	}
	
	vector_free(edges);
	return 0;
abort_depends:
	// * tag the vector as invalid
	dep_vect->flags |= DEPENDS_VECTOR_FLAG_INVALID;
	vector_foreach(edges, depends_edge_invalidate, NULL);
	vector_free(edges);
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
		if (edge->nodes[1]->satisified)
		{
			// edge is not satisfiable
			return;
		}
	}
	// is there a source node?
	if (edge->nodes[0])
	{
		// has the source node been satisfied?
		if (!edge->nodes[0]->satisfied)
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

