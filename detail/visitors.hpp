#ifndef _rlc_detail_visitors_hpp
#define _rlc_detail_visitors_hpp

#include <iterator>
#include "dag_node.hpp"
#include "scoped_flag.hpp"
#include "circular_reference_exception.hpp"

namespace rlc
{
	namespace detail
	{
		struct dag_node_visitor_empty_functor
		{
			template <class NodeType, class DataType>
			dag_node_visitor_empty_functor & operator()(NodeType & node, DataType & data)
			{
				return *this;
			}

			operator bool()
			{
				return false;
			}
		};
	
		struct dag_node_inc_score_functor
		{
			template <class NodeType, class DataType>
			dag_node_inc_score_functor & operator()(NodeType & node, DataType & data)
			{
				node.score_ += data;

				return *this;
			}

			operator bool()
			{
				return true;
			}
		};
	
		struct dag_node_dec_score_functor
		{
			template <class NodeType, class DataType>
			dag_node_dec_score_functor & operator()(NodeType & node, DataType & data)
			{
				node.score_ -= data;

				return *this;
			}

			operator bool()
			{
				return true;
			}
		};
		
		template <class NodeType, class BinaryFunction = dag_node_visitor_empty_functor, class DataType = void*>
		struct dag_node_visitor
		{
			dag_node_visitor()
			{
			}

			dag_node_visitor(const BinaryFunction & binary_function, const DataType data)
				: binary_function_(binary_function),
				  data_(data)
			{
			}
			
			dag_node_visitor & operator()(NodeType & node)
			{
				if (node.flags_ & NodeType::VISITED)
					throw circular_reference_exception();
				if (binary_function_)
					binary_function_(node, data_);
				{
					scoped_flag<NodeType> scoped_flag(node, NodeType::VISITED);
					std::for_each(
						node.targets_.begin(),
						node.targets_.end(),
						*this);
				}
				return *this;
			}
			dag_node_visitor & operator()(NodeType * node)
			{
				return (*this)(*node);
			}

			BinaryFunction binary_function_;
			DataType data_;
		};
	}
}

#endif

