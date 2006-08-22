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
#include "Node.h"

namespace Jail
{
	namespace Tree
	{
		//! Class modeling an Operator
		/** An operator binds together one or two expressions (or
		 * identifiers), depending on whether the operator is unary 
		 * or not. In any case, it corresponds to either a built-in
		 * operator or a function that overloads the operator.
		 * An operator can have one of two states: either it is resolved,
		 * or it isn't. If it's resolved, we know whether we should use
		 * a built-in operator or an overloaded function and we hold a
		 * pointer to it. If not resolved, the pointer is 0 */
		class Operator : public Node
		{
		public :
			enum OperatorType {
				//! (^) left-hand-side raised to the power of the right-hand-side
				power,
				//! (*) left-hand-side multiplied by the right-hand-side
				multiply,
				//! (/) left-hand-side divided by the right-hand-side
				divide,
				//! (%) remainder of the left-hand-side divided by the right-hand-side
				module,
				//! (+) right-hand-side added to the left-hand-side
				add,
				//! (-) right-hand-side substracted from the left-hand-side
				substract,
				//! (:=) right-hand-side assigned to the left-hand-side
				assign,
				//! (++x) left-hand-side incremented, incremented result returned
				pre_increment,
				//! (x++) left-hand-side incremented, result before incrementation returned
				post_increment,
				//! (--x) left-hand-side decremented, decremented result returned
				pre_decrement,
				//! (x--) left-hand-side decremented, result before decrementation returned
				post_decrement,
				//! (!x) logical negation of left-hand-side returned, left-hand-side unchanged
				logical_negate,
				//! (==) true if left-hand-side and right-hand-side are equal
				equality,
				//! (===) true if left-hand-side and right-hand-side are the same object
				same,
				//! (!=) true if left-hand-side and right-hand-side are unequal
				inequality,
				//! (!==) true if left-hand-side and right-hand-side are the not same object
				not_same,
				//! (<=) true if left-hand-side is less than or equal to the right-hand-side
				less_than_or_equal,
				//! (<) true if left-hand-side is less than the right-hand-side
				less_than,
				//! (>=) true if left-hand-side is greater than or equal to the right-hand-side
				greater_than_or_equal,
				//! (>) true if left-hand-side is greater than the right-hand-side
				greater_than
			};
			enum OperatorAssociation {
				//! left-to-right association (ex: any ary operator that does not assign)
				left_to_right,
				//! right-to-left association (ex: all unary operatos and the assignment operator (:=) )
				right_to_left
			};
			
			OperatorType getType() const;
			Type * getReturnType() const { return type_; }
			bool unary() const;
			bool resolved() const;

		private :
			Type * type_;
		};

		bool operator<(const Operator & oper1, const Operator & oper2);
	}
}

