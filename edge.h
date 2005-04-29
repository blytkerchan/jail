#ifndef _libdepends_edge_h
#define _libdepends_edge_h

#include <libcontain/list.h>
#include "node.h"

struct depends_edge_type
{
	/* the nodes this edge connects. nodes[0] is the node the edge source; nodes[1] the node it target. */
	depends_node_t * nodes[2];
	list_t * vectors;
	int invalid;
};
typedef struct depends_edge_type depends_edge_t;

depends_edge_t * depends_edge_new(depends_t * handle, depends_node_t * from, depends_node_t * to);
void depends_edge_free(depends_edge_t * edge);
int depends_edge_cmp(const void * k1, const void * k2);
depends_edge_t * depends_edge_find(depends_t * handle, depends_node_t * source, depends_node_t * target);
int depends_edge_insert(depends_t * handle, depends_edge_t * edge);
void depends_edge_invalidate(const void * ptr, void * p2);

#endif

