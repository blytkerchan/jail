/* Jail: Just Another Interpreted Language
 * Copyright (c) 2004, Ronald Landheer-Cieslak
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
#include <sys/types.h>
#include "prime.h"

static const unsigned int primes[] = {
	19, 29, 41, 59, 79, 107, 149, 197, 263, 347, 457, 599, 787, 1031,
	1361, 1777, 2333, 3037, 3967, 5167, 6719, 8737, 11369, 14783,
	19219, 24989, 32491, 42257, 54941, 71429, 92861, 120721, 156941,
	204047, 265271, 344857, 448321, 582821, 757693, 985003, 1280519,
	1664681, 2164111, 2813353, 3657361, 4754591, 6180989, 8035301,
	10445899, 13579681, 17653589, 22949669, 29834603, 38784989,
	50420551, 65546729, 85210757, 110774011, 144006217, 187208107,
	243370577, 316381771, 411296309, 534685237, 695090819, 903618083,
	1174703521, 1527114613, 1985248999, 2580823717UL, 3355070839UL
};

/** 
// get_prime(): get a prime greater than or equal to _U, return ~0 on error (_U > 3355070839)
*/
unsigned int get_prime(unsigned int _u) 
{
	size_t i;
	
	for (i = 0; i < (sizeof(primes) / sizeof(unsigned int)); i++)
	{
		if (primes[i] >= _u)
			return (primes[i]);
	} // for

	return (~0);
}

