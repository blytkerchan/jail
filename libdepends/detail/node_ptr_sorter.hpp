#ifndef _rlc_detail_node_ptr_sorter
#define _rlc_detail_node_ptr_sorter

namespace rlc
{
	namespace detail
	{
		template <typename NodeType>
		struct node_ptr_sorter
		{
			node_ptr_sorter() { /* no-op */ }
			~node_ptr_sorter() { /* no-op */ }

			bool operator()(NodeType * node1, NodeType * node2)
			{
				return node1->score_ < node2->score_;
			}
		};
	}
}

#endif

