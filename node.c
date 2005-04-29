#include "depends.h"
#include "node.h"
#include <assert.h>
#include <stdlib.h>
#include <libcontain/list.h>
#include <libcontain/vector.h>

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

struct depends_node_visitor_helper_data_type
{
	int fail;
};
typedef struct depends_node_visitor_helper_data_type depends_node_visitor_helper_data_t;

static void depends_node_visit_helper(const void * edge, void * data);
int depends_node_visit(depends_node_t * node, depends_node_visitor_func visitor, void * d)
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

depends_node_t * depends_node_new(depends_t * handle, const void * key)
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

void depends_node_free(depends_node_t * node)
{
	list_free(node->edges[0]);
	list_free(node->edges[1]);
	free(node);
}

int depends_node_cmp(const void * k1, const void * k2)
{
	depends_node_t * node1 = (depends_node_t*)k1;
	depends_node_t * node2 = (depends_node_t*)k2;
	
	return node1->dep_key_cmp(node1->key, node2->key);
}

depends_node_t * depends_node_find(depends_t * handle, const void * key)
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


