#ifndef _rlc_detail_circular_reference_exception_h
#define _rlc_detail_circular_reference_exception_h

#include <stdexcept>

namespace rlc
{
	namespace detail
	{
		struct circular_reference_exception : public std::logic_error
		{
			circular_reference_exception()
				: std::logic_error("circular reference")
			{
			}
		};
	}
}

#endif

