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
		class Identifier;
		class Operator;
		class Type;
		
		//! Node modeling an expression
		/** An expression is a note with one or two identifiers and an operator
		 * as children and a return type. If the operator is unary, there is only
		 * one identifier. Otherwise, there are two. The return type of the
		 * expression depends on the operator used. Hence, the Expression class
		 * basically only wraps the Operator and Identifier instances together.
		 * However, the Expression can also be compound - i.e. link together
		 * (with an operator) an identifier with an expression, and expression
		 * with an identifier, or two expressions. In that case, the operator
		 * cannot be unary. */
		class Expression : public Node
		{
		public :
			Expression(Operator * oper, Identifier * lhs, Identifier * rhs = 0);
			Expression(Operator * oper, Identifier * lhs, Expression * rhs);
			Expression(Operator * oper, Expression * lhs, Identifier * rhs);
			Expression(Operator * oper, Expression * lhs, Expression * rhs);
			Type * getType() const { return type_; }
			
		private :
			Type * type_;
			Operator * operator_;
			ExpressionOrIdentifier * lhs_;
			ExpressionOrIdentifier * rhs_;
		};
	}
}

