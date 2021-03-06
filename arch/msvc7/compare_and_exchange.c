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
#include "arch/compare_and_exchange.h"

int compare_and_exchange_int(int32_t * exp_ptr, volatile int32_t * tar_ptr, int32_t src_ptr)
{
	int retval = 0;

	__asm mov eax, DWORD PTR [exp_ptr]
	__asm mov ebx, DWORD PTR [eax]
	__asm mov ecx, DWORD PTR [tar_ptr]
	__asm mov edx, DWORD PTR [src_ptr]
	__asm mov eax, ebx
	__asm lock cmpxchg DWORD PTR [ecx], edx
	__asm jz done
	__asm mov ebx, DWORD PTR [exp_ptr]
	__asm mov DWORD PTR [ebx], eax
	__asm mov retval, -1
done :

	return retval;
}

int compare_and_exchange_ptr(void * exp_ptr, volatile void * tar_ptr, const void * src_ptr)
{
	int retval = 0;

	__asm mov eax, DWORD PTR [exp_ptr]
	__asm mov ebx, DWORD PTR [eax]
	__asm mov ecx, DWORD PTR [tar_ptr]
	__asm mov edx, DWORD PTR [src_ptr]
	__asm mov eax, ebx
	__asm lock cmpxchg DWORD PTR [ecx], edx
	__asm jz done
	__asm mov ebx, DWORD PTR [exp_ptr]
	__asm mov DWORD PTR [ebx], eax
	__asm mov retval, -1
done :

	return retval;
}
