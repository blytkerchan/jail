// for the time being, we'll use namespace rlc in stead of boost..
#ifndef _rlc_dag_hpp
#define _rlc_dag_hpp

#include <vector>
#include "detail/dag_iterator.hpp"
#include "detail/dag_node.hpp"
#include "detail/circular_reference_exception.hpp"
#include "detail/scoped_flag.hpp"
#include "detail/visitors.hpp"

namespace rlc
{
	// rlc::dag is a Reversible Container and a Unique Associative Container
	// Note that we don't have a way to specify the allocator to use yet
	template <class ValueType>
	class dag
	{
	public :
		typedef ValueType value_type;
		typedef ValueType key_type;
		typedef ValueType & reference;
		typedef const ValueType & const_reference;
		typedef ValueType * pointer;
		typedef const ValueType * const_pointer;
		typedef unsigned long score_type;
		typedef detail::dag_node<ValueType, score_type> node_type;
		typedef detail::dag_iterator<ValueType, const ValueType &, const ValueType *, score_type, typename std::vector<node_type>::iterator> iterator;
		typedef detail::dag_iterator<ValueType, const ValueType &, const ValueType *, score_type, typename std::vector<node_type>::iterator> const_iterator;
		typedef detail::dag_reverse_iterator<ValueType, const ValueType &, const ValueType *, score_type, typename std::vector<node_type>::reverse_iterator> reverse_iterator;
		typedef detail::dag_reverse_iterator<ValueType, const ValueType &, const ValueType *, score_type, typename std::vector<node_type>::reverse_iterator> const_reverse_iterator;
		typedef typename std::vector<node_type>::difference_type difference_type;
		typedef typename std::vector<node_type>::size_type size_type;

		typedef detail::circular_reference_exception circular_reference_exception;

		dag() {}
		// we are assigable
		dag(const dag & d) : nodes_(d.nodes) {}
		template <typename InputIterator>
		dag(InputIterator first, InputIterator last)
		{
			insert(first, last);
		}
		
		dag & operator=(const dag & d) { nodes_ = d.nodes_; return *this; }
	
		// we're a reversible container
		iterator begin() { return iterator(nodes_.begin()); }
		const_iterator begin() const { return const_iterator(nodes_.begin()); }
		reverse_iterator rbegin() { return reverse_iterator(nodes_.rbegin()); }
		const_reverse_iterator rbegin() const { return const_reverse_iterator(nodes_.rbegin()); }

		iterator end() { return iterator(nodes_.end()); }
		const_iterator end() const { return const_iterator(nodes_.end()); }
		reverse_iterator rend() { return reverse_iterator(nodes_.rend()); }
		const_reverse_iterator rend() const { return const_reverse_iterator(nodes_.rend()); }

		size_type size() const { return nodes_.size(); }
		size_type max_size() const { return sizeof(score_type) > sizeof(size_type) ? (size_type)~0 : (score_type)~0; }

		bool empty() const { return nodes_.empty(); }
		void swap(dag & d) { nodes_.swap(d.nodes_); }

		bool operator==(const dag & d) const { return nodes_ == d.nodes_; }
		bool operator!=(const dag & d) const { return nodes_ != d.nodes_; }

		bool operator<(const dag & d) const { return nodes_ < d.nodes_; }
		bool operator>(const dag & d) const { return nodes_ > d.nodes_; }
		
		bool operator<=(const dag & d) const { return nodes_ <= d.nodes_; }
		bool operator>=(const dag & d) const { return nodes_ >= d.nodes_; }

		std::pair<iterator, bool> insert(const value_type & val)
		{
			if (std::find_if(nodes_.begin(), nodes_.end(), detail::dag_node_value_cmp<node_type>(val)) == nodes_.end())
			{
				nodes_.insert(nodes_.begin(), node_type(val));
				return std::make_pair(begin(), true);
			}
			else
				return std::make_pair(end(), false);
		}

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last)
		{
			for ( ; first != last; ++first)
				insert(*first);
		}
		
		void link(iterator source, iterator target)
		{
			{
				detail::scoped_flag<node_type> scoped_flag(source.node(), node_type::VISITED);
				detail::dag_node_visitor<node_type>()(target.node());
			}
			source.node().targets_.push_back(&target.node());

			// GCC 3.3 doesn't like it when I don't use a local variable for this functor..
			detail::dag_node_inc_score_functor dag_node_inc_score_functor;
			detail::dag_node_visitor<node_type, detail::dag_node_inc_score_functor, score_type>(dag_node_inc_score_functor, source.node().score_)(target.node());

			std::sort(nodes_.begin(), nodes_.end());
		}

		void link(iterator source, value_type target)
		{
			iterator target_iter = std::find(begin(), end(), target);

			if (target_iter == end())
				throw std::invalid_argument("value not found");
			link(source, target_iter);
		}

		void link(value_type source, iterator target)
		{
			iterator source_iter = std::find(begin(), end(), source);

			if (source_iter == end())
				throw std::invalid_argument("value not found");
			link(source_iter, target);
		}

		void link(value_type source, value_type target)
		{
			iterator source_iter = std::find(begin(), end(), source);
			iterator target_iter = std::find(begin(), end(), target);

			if (source_iter == end() || target_iter == end())
				throw std::invalid_argument("value not found");
			link(source_iter, target_iter);
		}
	
		bool linked(iterator source, iterator target) const
		{
			try
			{
				detail::scoped_flag<node_type> scoped_flag(target.node(), node_type::VISITED);
				detail::dag_node_visitor<node_type>()(source.node());
			}
			catch (detail::circular_reference_exception &)
			{
				return true;
			}

			return false;
		}

		bool linked(iterator source, value_type target) const
		{
			iterator target_iter = std::find(begin(), end(), target);

			if (target_iter == end())
				return false;
			return linked(source, target_iter);
		}

		bool linked(value_type source, iterator target) const
		{
			iterator source_iter = std::find(begin(), end(), source);

			if (source_iter == end())
				return false;
			return linked(source_iter, target);
		}

		bool linked(value_type source, value_type target) const
		{
			iterator source_iter = std::find(begin(), end(), source);
			iterator target_iter = std::find(begin(), end(), target);

			if (source_iter == end() || target_iter == end())
				return false;
			return linked(source_iter, target_iter);
		}

		bool unlink(iterator source, iterator target)
		{
			bool rv = source.node().targets_.erase(std::find(source.node().targets_.begin(), source.node().targets_.end(), &(target.node()))) != source.node().targets_.end();

			// GCC 3.3 doesn't like it when I don't use a local variable for this functor..
			detail::dag_node_dec_score_functor dag_node_dec_score_functor;
			detail::dag_node_visitor<node_type, detail::dag_node_dec_score_functor, score_type>(dag_node_dec_score_functor, source.node().score_)(target.node());

			std::sort(nodes_.begin(), nodes_.end());

			return rv;
		}
		
		bool unlink(iterator source, value_type target)
		{
			iterator target_iter = std::find(begin(), end(), target);

			if (target_iter == end())
				throw std::invalid_argument("value not found");
			return unlink(source, target_iter);
		}

		bool unlink(value_type source, iterator target)
		{
			iterator source_iter = std::find(begin(), end(), source);

			if (source_iter == end())
				throw std::invalid_argument("value not found");
			return unlink(source_iter, target);
		}

		bool unlink(value_type source, value_type target)
		{
			iterator source_iter = std::find(begin(), end(), source);
			iterator target_iter = std::find(begin(), end(), target);

			if (source_iter == end() || target_iter == end())
				throw std::invalid_argument("value not found");
			return unlink(source_iter, target_iter);
		}

		iterator erase(iterator where)
		{
			while (!where.node().targets_.empty())
				unlink(where, *(where.node().targets_[0]));
			iterator remove = where++;
			nodes_.erase(nodes_.begin(), nodes_.end(), std::find(remove.node()));

			return where;
		}
		
		iterator erase(iterator begin, iterator end)
		{
			iterator retval = begin;
			
			while (retval = erase(retval) != end);

			return retval;
		}
		
	private :
		mutable std::vector< node_type > nodes_;
	};
}

#endif

