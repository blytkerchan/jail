#ifndef _rlc_detail_dag_iterator_hpp
#define _rlc_detail_dag_iterator_hpp

#include <iterator>
#include "dag_node.hpp"

namespace rlc
{
	namespace detail
	{
		struct reverse_tag {};
		
		template <class ValueType, typename ReferenceType, typename PointerType, typename ScoreType, typename IteratorType>
		struct dag_iterator_base : public std::iterator<std::forward_iterator_tag, ValueType, ptrdiff_t, PointerType, ReferenceType>
		{
			typedef dag_iterator_base<ValueType, ValueType&, ValueType*, ScoreType, IteratorType> iterator;
			typedef dag_iterator_base<ValueType, const ValueType&, const ValueType*, ScoreType, IteratorType> const_iterator;
			typedef dag_node<ValueType, ScoreType> node_type;
			
			dag_iterator_base(const IteratorType & i) : iter_(i) {}
			dag_iterator_base(const iterator & i) : iter_(i.iter_) {}

			ReferenceType operator*() const { return iter_->value_; }
			PointerType operator->() const { return &(iter_->value_); }

			bool operator==(const dag_iterator_base & i) const { return iter_ == i.iter_; }
			bool operator!=(const dag_iterator_base & i) const { return iter_ != i.iter_; }

			const node_type & node() const { return *iter_; }
			node_type & node() { return *iter_; }

//		protected :
			IteratorType iter_;
		};

		template <class ValueType, typename ReferenceType, typename PointerType, typename ScoreType, typename IteratorType>
		struct dag_iterator : public dag_iterator_base<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>
		{
			typedef dag_iterator<ValueType, ValueType&, ValueType*, ScoreType, IteratorType> iterator;
			typedef dag_iterator<ValueType, const ValueType&, const ValueType*, ScoreType, IteratorType> const_iterator;
			typedef dag_iterator_base<ValueType, ReferenceType, PointerType, ScoreType, IteratorType> super;
			
			dag_iterator(const IteratorType & i) : dag_iterator_base<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>(i) {}
			dag_iterator(const iterator & i) : dag_iterator_base<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>(i) {}

			dag_iterator & operator++() { ++(super::iter_); return *this; };
			dag_iterator operator++(int) { dag_iterator tmp = *this; ++(super::iter_); return tmp; };
		};

		template <class ValueType, typename ReferenceType, typename PointerType, typename ScoreType, typename IteratorType>
		struct dag_reverse_iterator : public dag_iterator_base<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>
		{
			typedef dag_reverse_iterator<ValueType, ValueType&, ValueType*, ScoreType, IteratorType> iterator;
			typedef dag_reverse_iterator<ValueType, const ValueType&, const ValueType*, ScoreType, IteratorType> const_iterator;
			typedef dag_iterator_base<ValueType, ReferenceType, PointerType, ScoreType, IteratorType> super;
			
			dag_reverse_iterator(const IteratorType & i) : dag_iterator_base<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>(i) {}
			dag_reverse_iterator(const iterator & i) : dag_iterator_base<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>(i) {}

			dag_reverse_iterator & operator++() { ++(super::iter_); return *this; };
			dag_reverse_iterator operator++(int) { dag_reverse_iterator tmp = *this; ++(super::iter_); return tmp; };
		};
	}
}

#endif

