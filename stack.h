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
#ifndef _LIBCONTAIN_STACK_H
#define _LIBCONTAIN_STACK_H

typedef struct stack_node_t {
	void 	* val;
	struct stack_node_t * next;
	int removed;
} stack_node_t;

typedef struct stack_t
{
	stack_node_t	* top;
} stack_t;

/* All stack operations are guaranteed to be thread-safe, meaning you can do 
 * them all on the same stack at the same time, if you want.
 * Depending on architectural constraints, the implementation may or may not be 
 * non-blocking - the algorithm used is non-blocking in any case. 
 * Behaviour of any of these functions when given an invalid stack descriptor
 * (such as a NULL pointer) is undefined 
 * The "stack descriptor" is a pointer to the stack_t type. This type should
 * be considered opaque and non-portable. */

/* new_stack: make a new stack.
 * This function guarantees:
 * * that any resources associated with the new stack are uniquely associated 
 *   with its descriptor
 * * that the new stack descriptor is unique to the current process
 */
stack_t * new_stack(void);

/* free_stack: free the stack and the resources associated with it.
 * This function guarantees:
 * * that any resources associated with the stack itself are freed
 * * that any resources associated with the values contained in the stack will
 *   not be freed and will remain associated with those values
 * * that the stack's descriptor will no longer be valid at some time during 
 *   the execution of the function
 */
void free_stack(stack_t * stack);

/* stack_top: get the top value from the stack.
 * This function guarantees:
 * * to return NULL if there are no non-deleted values in the stack
 * * that the value otherwise returned was on the top of the stack at
 *   some moment during the execution of the function
 * * that it will not change the status nor the contents of the stack.
 * This function DOES NOT guarantee:
 * * that the value returned is still the first non-deleted value at the time
 *   it is returned
 * * that the value returned has not been deleted by the time it is returned.
 * * not to do *any* write operations if they are guaranteed to cancel 
 *   eachother out
 */
void * stack_top(stack_t * stack);

/* stack_pop: remove the first item from the stack.
 * This function guarantees:
 * * to delete the top element of the stack if an only if it has not been 
 *   deleted yet
 * * to return the number of items deleted from the stack (0 or 1)
 * This function DOES NOT guarantee:
 * * that if it does not remove anything from the stack, the stack is empty
 */
int stack_pop(stack_t * stack);

/* stack_push: put something on top of the stack.
 * This function guarantees:
 * * that the value passed into/added to the stack shall only be read and 
 *   shall not be written to
 * * that the value shall be passed above an element in the stack that was on
 *   its top at some time during its execution
 * This function DOES NOT guarantee:
 * * that when the function returns the added value shall still be at the top
 *   position on the stack.
 */
void stack_push(stack_t * stack, void * val);

#endif // _LIBCONTAIN_STACK_H
