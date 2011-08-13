#ifndef _rlc_detail_dag_node_hpp
#define _rlc_detail_dag_node_hpp

namespace rlc
{
	namespace detail
	{
		template <typename NodeType>
		struct dag_node_value_cmp
		{
			typedef typename NodeType::value_type value_type;

			dag_node_value_cmp(const value_type & v) : val_(v) {}

			bool operator()(const value_type & v) const { return val_ == v; }
			bool operator()(const NodeType & n) const { return n.value_ == val_; }
			bool operator()(const NodeType * n) const { return n->value_ == val_; }
			
			value_type val_;
		};
		
		template <class ValueType, typename ScoreType>
		struct dag_node
		{
			typedef ValueType value_type;
			typedef ScoreType score_type;
			typedef std::vector<dag_node*> targets_type;
			
			enum flag_type { VISITED = 1 };

			dag_node(const ValueType & v)
				: value_(v),
				  score_(1),
				  flags_(0)
			{
			}

			bool operator<(const dag_node & n) const
			{
				return score_ < n.score_;
			}
			
			targets_type targets_;
			ValueType value_;
			ScoreType score_;
			unsigned int flags_;
		};
	}
}

#endif

