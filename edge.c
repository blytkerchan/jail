#include "depends.h"
#include "handle.h"
#include "edge.h"
#include "node.h"
#include "vector.h"
#include <assert.h>
#include <stdlib.h>
#include <libcontain/list.h>

depends_edge_t * depends_edge_new(depends_t * handle, depends_node_t * from, depends_node_t * to)
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

void depends_edge_free(depends_edge_t * edge)
{
	list_free(edge->vectors);
	free(edge);
}

int depends_edge_cmp(const void * k1, const void * k2)
{
	depends_edge_t * edge1 = (depends_edge_t*)k1;
	depends_edge_t * edge2 = (depends_edge_t*)k2;
	
	if (edge1->nodes[0] != edge2->nodes[0])
		return edge1->nodes[0] - edge2->nodes[0];
	if (edge1->nodes[1] != edge2->nodes[1])
		return edge1->nodes[1] - edge2->nodes[1];
	
	return 0;
}

depends_edge_t * depends_edge_find(depends_t * handle, depends_node_t * source, depends_node_t * target)
{
	depends_edge_t t_edge;
	t_edge.nodes[0] = source;
	t_edge.nodes[1] = target;
	
	return list_search(source->edges[0], &t_edge);
	
}

int depends_edge_insert(depends_t * handle, depends_edge_t * edge)
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

void depends_edge_invalidate(const void * ptr, void * p2)
{
	depends_edge_t * edge = (depends_edge_t*)ptr;
	edge->invalid = 1;
}

