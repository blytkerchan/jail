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
namespace Jail
{
	namespace Tree
	{
		class Expression;
		class Identifier;
	
		//! Wrapper class around an expression or an identifier
		/** In some cases, expressions and identifiers may be inter-
		 * changeable as both may appear in the same place in the
		 * parse tree. In those cases, you can use this class as a
		 * place-holder for either.
		 * This class has no mutators - only constructors and accessors.
		 * This is by design: you cannot create an instance of this
		 * class without knowing what you're going to put in it and
		 * you can't change its contents afterwards. */
		class ExpressionOrIdentifier
		{
		public :
			//! Construct from an expression
			ExpressionOrIdentifier(Expression * expression) : expression_(expression), identifier_(0) {}
			//! Construct from an identifier
			ExpressionOrIdentifier(Identifier * identifier) : expression_(0), identifier_(identifier) {}

			//! Returns true if it holds an expression
			bool isExpression() const { return expression_ != 0; }
			//! Returns true if it holds an identifier
			bool isIdentifier() const { return identifier_ != 0; }

			//! Returns the expression, or 0 if none
			Expression * getExpression() { return expression_; }
			//! Returns the identifier, or 0 if none
			Identifier * getIdentifier() { return identifier_; }

		private :
			Expression * expression_;
			Identifier * identifier_;
		};
	}
}

