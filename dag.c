/* Jail: Just Another Interpreted Language
 * Copyright (c) 2004, Ronald Landheer-Cieslak
 * All rights reserved
 * 
 * This is free software. You may distribute it and/or modify it and
 * distribute modified forms provided that the following terms are met:
 *
 * * Redistributions of the source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer;
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the distribution;
 * * None of the names of the authors of this software may be used to endorse
 *   or promote this software, derived software or any distribution of this 
 *   software or any distribution of which this software is part, without 
 *   prior written permission from the authors involved;
 * * Unless you have received a written statement from Ronald Landheer-Cieslak
 *   that says otherwise, the terms of the GNU General Public License, as 
 *   published by the Free Software Foundation, version 2 or (at your option)
 *   any later version, also apply.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "dag.h"
#include <stdlib.h>

#define DAG_NODE_VISITED	1

struct dag_node_type
{
	/* edges[0] are the incoming edges, edges[1] the outgoing edges */
	dag_edge_t ** edges[2];
	int flags;
};

struct dag_edge_type
{
	/* nodes[0] is the source node, nodes[1] the target node */
	dag_node_t * nodes[2];
}

dag_node_t * dag_node_new(void)
{
	dag_node_t * retval = malloc(sizeof(dag_node_t));
	if (retval == NULL)
		return NULL;
	
	retval->edges[0] = NULL;
	retval->edges[1] = NULL;
	
	return retval;
}

void dag_node_free(dag_node_t * node)
{
	free(node->edges[0]);
	free(node->edges[1]);
	free(node);
}

dag_edge_t * dag_edge_new(void)
{
	dag_edge_t * retval = malloc(sizeof(dag_edge_t));
	if (retval == NULL)
		return NULL;
	retval->nodes[0] = NULL;
	retval->nodes[1] = NULL;

	return retval;
}

void dag_edge_free(dag_edge_t * edge)
{
	free(edge);
}

int dag_node_link(dag_node_t * source, dag_node_t * target)
{
	int rv, i;
	dag_edge_t * edge;
	dag_edge_t ** outgoing;
	dag_edge_t ** incoming;
	unsigned int n_outgoing, n_incoming;
	
	assert(source && target);
	source->flags |= DAG_NODE_VISITED;
	rv = dag_node_visit(target, NULL, NULL);
	source->flags ^= DAG_NODE_VISITED;
	if (rv != 0)
		return -1;	// would create a circular dependency
	for (i = 0; source->edges[1] && source->edges[1][i]; ++i)
	{
		if (source->edges[1][i]->nodes[1] == target)
			return 0;	// edge already exists - not an error
	}
	edge = dag_edge_new();
	if (!edge)
		return -1;
	edge[0] = source;
	edge[1] = target;
	for (n_outgoing = 0, source->edges[1] && source->edges[1][n_outgoing], n_outgoing++);
	for (n_incoming = 0, target->edges[0] && target->edges[0][n_incoming], n_incoming++);
	/* NOTE: the standard specifies that if the reallocation does not
	 * occur, "the object shall remain unchanged" */
	outgoing = realloc(source->edges[1], (n_outgoing + 2) * sizeof(dag_edge_t*));
	if (outgoing == NULL)
	{
		dag_edge_free(edge);
		return -1;
	}
	source->edges[1] = outgoing;
	incoming = realloc(target->edges[0], (n_incoming + 2) * sizeof(dag_edge_t*));
	if (outgoing == NULL)
	{
		dag_edge_free(edge);
		return -1;
	}
	target->edges[0] = incoming;
	source->edges[1][n_outgoing] = edge;
	source->edges[1][n_outgoing + 1] = NULL;
	target->edges[0][n_incoming] = edge;
	target->edges[0][n_incoming + 1] = NULL;

	return 0;
}

int dag_node_unlink(dag_node_t * source, dag_node_t * target);

int dag_node_visit(dag_node_t * node, dag_node_visitor_func visitor, void * data);


