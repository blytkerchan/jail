#ifndef _singleton_h_
#define _singleton_h_

#include <exception>
#include <cassert>
#ifdef _MSC_VER
#pragma warning(disable: 4311)
#include <windows.h>
#define atomic_increment InterlockedIncrement
#define atomic_decrement InterlockedDecrement
static inline int compare_and_exchange(void *e, void *d, void *s)
{																						
	long retval;																		
																						
	retval = InterlockedCompareExchange((volatile long *)d, (long)s, *(long *)e);		
	if (retval == *(long*)e)															
		return 0;																		
	else																				
		return 1;																		
}
#define uint32_t long
#define sleep Sleep
#else
#include "increment.h"
#include "decrement.h"
#include "compare_and_exchange.h"
#endif
#ifndef NULL
#define NULL 0
#endif

#define SINGLETON_LOCK()									\
{												\
	unsigned int lock = 0;									\
												\
	while (1)										\
	{											\
		while (compare_and_exchange((void*)&lock, (void*)&lock_, (void*)1) != 0)	\
			lock = 0;								\
		if (readers_ > 0)								\
			atomic_decrement((uint32_t*)&lock_);					\
		else break;									\
		sleep(0);									\
	}											\
}												\
try												\
{								
#define SINGLETON_UNLOCK()						\
}									\
catch (...)								\
{									\
	atomic_decrement((uint32_t*)&lock_);				\
	throw;								\
}									\
atomic_decrement((uint32_t*)&lock_);
#define SINGLETON_REG_READER()						\
{									\
	while (1)							\
	{								\
		atomic_increment((uint32_t*)&readers_);			\
		if (lock_)						\
			atomic_decrement((uint32_t*)&readers_);		\
		else break;						\
		sleep(0);						\
	}								\
}									\
try									\
{
#define SINGLETON_UNREG_READER()					\
}									\
catch (...)								\
{									\
	atomic_decrement((uint32_t*)&readers_);				\
	throw;								\
}									\
atomic_decrement((uint32_t*)&readers_);

namespace rlc
{
	template <typename T, unsigned int instance = 0>
	class singleton
	{
	public :
		typedef T element_type;

		singleton(void)
		{
			SINGLETON_REG_READER();
			atomic_increment((uint32_t *)&ref_count_);
			SINGLETON_UNREG_READER();
		}

		singleton(const singleton<T, instance> & _s)
		{
			SINGLETON_REG_READER();
			assert(_s.ptr_ == ptr_);
			atomic_increment((uint32_t *)&ref_count_);
			SINGLETON_UNREG_READER();
		}

		singleton(T * ptr)
		{
			T * exp = NULL;

			SINGLETON_LOCK();
			if (compare_and_exchange(&exp, &ptr_, ptr) != 0)
			{	// the pointer was not NULL - throw an exception
				throw "Re-defining a singleton pointer!";
			}
			atomic_increment((uint32_t*)&ref_count_);
			SINGLETON_UNLOCK();
		}
		
		~singleton(void)
		{
			SINGLETON_LOCK();
			atomic_decrement((uint32_t*)&ref_count_);
			if (ref_count_ == 0)
			{	// last reference to the singleton has expired - we can remove it
				T * ptr = NULL;
				T * exp = ptr_;
				 
				assert(compare_and_exchange((void*)&exp, (void*)&ptr_, (void*)ptr) == 0);
			}
			SINGLETON_UNLOCK()
		}
		
		singleton & operator=(const singleton<T, instance> * _s)
		{
			SINGLETON_REG_READER();
			assert(ptr_ == _s.ptr_);
			SINGLETON_UNREG_READER();

			return *this;
		}

		T & operator*() const
		{
			T * retval;
			retval = get();

			return *retval;
		}

		T * operator->() const
		{
			return get();
		}

		T * operator->()
		{
			return get(); 
		}

		T * get() const
		{
			T * retval;
			SINGLETON_REG_READER();
			retval = ptr_;
			SINGLETON_UNREG_READER();
			return retval; 
		}

		void reset()
		{
			SINGLETON_LOCK();
			delete ptr_;
			ptr_ = NULL;
			SINGLETON_UNLOCK();
		}

		operator void*() const
		{
			return get();
		}
	private :
		static T * ptr_;
		static volatile unsigned int ref_count_;
		static volatile unsigned int lock_;
		static volatile unsigned int readers_;
	};

	template <typename T, unsigned int instance>
	T * singleton<T, instance>::ptr_ = NULL;
	template <typename T, unsigned int instance>
	volatile unsigned int singleton<T, instance>::ref_count_ = 0;
	template <typename T, unsigned int instance>
	volatile unsigned int singleton<T, instance>::lock_ = 0;
	template <typename T, unsigned int instance>
	volatile unsigned int singleton<T, instance>::readers_ = 0;
} // namespace rlc

#endif // ! _singleton_h_
