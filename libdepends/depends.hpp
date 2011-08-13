#ifndef _rlc_depends_hpp
#define _rlc_depends_hpp

#include <algorithm>
#include <stdexcept>
#include "dag.hpp"

namespace rlc
{
	template <class T>
	class depends
	{
	public :
		struct add_exception : public std::runtime_error
		{
			add_exception() : std::runtime_error("add failed") {}
		}
		struct select_exception : public std::runtime_error
		{
			select_exception() : std::runtime_error("select failed") {}
		}
		
		typedef dag<T> dag_type;
		
		depends() : selected_(false) {}
		template <class InputIterator>
		depends(InputIterator begin, InputIterator end) : dag_(begin, end), selected_(false) {}
		depends(const depends & dep) : dag_(dep.dag_), selected_(false) {}
		depends & operator=(const depends & dep) { dag_ = dep.dag_; return *this; }
		~depends();
		
		void add(const T & key)
		{
			std::pair<dag_type::iterator, bool> rv = dag_.insert(key);
			if (!rv.second)
				throw add_exception();
		}
		
		void select(const T & key)
		{
			current_ = std::find(dag_.begin(), dag_.end(), key);
			if (current_ == dag_.end())
				throw select_exception();
			selected_ = true;
		}
		
		void depends(const T & target)
		{
			assert(selected_);
			dag_.link(current_, target);
		}
		
		template <class InputIterator>
		void depends(InputIterator begin, InputIterator end)
		{
			for (; begin != end; ++begin)
				depends(*begin);
		}
		
		bool depends(const T & target) const
		{
			assert(selected_);
			return dag_.linked(current_, target);
		}
		
		template <class InputIterator>
		bool depends(InputIterator begin, InputIterator end) const
		{
			for ( ; begin != end; ++begin)
				if (!depends(*begin))
					return false;
			return true;
		}
		
		void blocks(const T & source)
		{
			assert(selected_);
			dag_.link(source, current_);
		}
		
		template <class InputIterator>
		void blocks(InputIterator begin, InputIterator end)
		{
			for ( ; begin != end; ++begin)
				blocks(*begin);
		}
		
		bool blocks(const T & source) const
		{
			assert(selected_);
			return dag_.linked(source, current_);
		}
		
		template <class InputIterator>
		bool blocks(InputIterator begin, InputIterator end) const
		{
			for ( ; begin != end; ++begin)
				if (!blocks(*begin))
					return false;
			return true;
		}
		
		void resolve()
		{
			assert(selected_);
			dag_.erase(current_);
			selected_ = false;
		}

	private :
		dag_type dag_;
		dag_type::iterator current_;
		bool selected_;
	};
}

#endif

