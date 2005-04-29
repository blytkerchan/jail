#ifndef _libdepends_node_h
#define _libdepends_node_h

#include <libcontain/list.h>

#define DEPENDS_NODE_SATISFIED		0x00000001
#define DEPENDS_NODE_FLAG_VISITED       0x00000002

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
typedef struct depends_node_type depends_node_t;
typedef void (*depends_node_visitor_func)(depends_node_t *, void * data);

int depends_node_visit(depends_node_t * node, depends_node_visitor_func visitor, void * d);
depends_node_t * depends_node_new(depends_t * handle, const void * key);
void depends_node_free(depends_node_t * node);
int depends_node_cmp(const void * k1, const void * k2);
depends_node_t * depends_node_find(depends_t * handle, const void * key);

#endif

