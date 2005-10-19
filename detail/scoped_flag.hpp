#ifndef _rlc_detail_scoped_flag_hpp
#define _rlc_detail_scoped_flag_hpp

namespace rlc
{
	namespace detail
	{
		template <class NodeType, typename FlagType = unsigned int>
		struct scoped_flag
		{
			scoped_flag(NodeType * node, FlagType flag)
				: node_(node),
				  flag_(flag)
			{
				node->flags_ |= flag;
			}

			~scoped_flag()
			{
				node_->flags_ ^= flag_;
				node_->flags_ &= ~flag_;
			}

			NodeType * node_;
			FlagType flag_;
		};
	}
}

#endif

