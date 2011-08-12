/* Jail: Just Another Interpreted Language
 * Copyright (c) 2003-2004, Ronald Landheer-Cieslak
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
#include <unistd.h>
#include "arch/increment.h"
#include "arch/decrement.h"

/* As of the i486, x86 processors have a compare and exchange function
 * which will evidently be called with the same prototype of i486+ 
 * computers. However, on the off chance that someone might want to use
 * something older than an i486 (or a non-x86 platform for which the 
 * architecture-specific routine has not been written yet) we provide
 * a blocking implementation in C.
 * This implementation is not intended to be very efficient: we synchronize
 * on a function-level without watching too closely what we'll be working on.
 * If you want a better-optimized version, feel free to contribute one :)
 */
int compare_and_exchange(void ** exp_ptr, void ** tar_ptr, const void * src_ptr)
{
	static int lock = 0;
	int rv;

	atomic_increment(&lock);
	while (lock != 1) sleep(0);

	if (*exp_ptr != *tar_ptr)
	{
		*exp_ptr = *tar_ptr;
		rv = -1;
	}
	else
	{
		*tar_ptr = src_ptr;
		rv = 0;
	}
	
	atomic_decrement(&lock);
	return(rv);
}


