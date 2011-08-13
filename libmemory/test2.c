/* Jail: Just Another Interpreted Language
 * Copyright (c) 2003, Ronald Landheer-Cieslak
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
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "smr.h"
#define N_THREADS 	10
#define N_ALLOCS 	5
#define ALLOC_SIZE	100
#define MAIN_TIMEOUT	60

typedef struct _thread_stat_t
{
	pthread_t id;
	int flag;
	int * main_flag;
} thread_stat_t;

void * thread_func(void * param)
{
	thread_stat_t * thread_stat = (thread_stat_t*)param;
	int i;
	void * allocs[N_ALLOCS];

	do
	{
		for (i = 0; i < N_ALLOCS; i++)
		{
			allocs[i] = malloc(ALLOC_SIZE);
		}
		for (i = 0; i < N_ALLOCS; i++)
		{
			free(allocs[i]);
		}
	} while (*(thread_stat->main_flag) != 0);
	thread_stat->flag = 1;
	
	return NULL;
}

int main(void)
{
	smr_global_data_t smr_config;
	thread_stat_t * thread_stat[N_THREADS];
	int t;
	int flag;

	smr_init(N_ALLOCS);

	flag = 1;
	for (t = 0; t < N_THREADS; t++)
	{
		thread_stat[t] = (thread_stat_t*)malloc(sizeof(thread_stat_t));
		thread_stat[t]->flag = 0;
		thread_stat[t]->main_flag = &flag;
		pthread_create(&(thread_stat[t]->id), NULL, thread_func, thread_stat[t]);
	}
	sleep(MAIN_TIMEOUT);
	flag = 0;
	for (t = 0; t < N_THREADS; t++)
	{
		while (thread_stat[t]->flag == 0)
			sleep(0);
		pthread_join(thread_stat[t]->id, NULL);
	}
	
	return(0);
}

