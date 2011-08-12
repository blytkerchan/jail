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
#ifndef _visitors_visitor_h
#define _visitors_visitor_h

#include "SubjectTraits.h"
#include "TraversalPolicy.h"

namespace Visitors
{
	//! The Visitor class
	/** This class wraps the traits and policy together to make them into
	 * a functional implementation of the Visitor pattern. Normally, you'd
	 * want to use it like this:
	 * \code
	 * typedef Visitor< MySubject, MyTraversalPolicy > MyVisitor;
	 * ...
	 * void foo()
	 * {
	 * 	...
	 * 	MyVisitor my_visitor;
	 * 	my_visitor.setData(my_data);
	 * 	my_visitor.visit(root_node_of_my_hierarchy);
	 * 	// now get anything the visitor may have found during its visit..
	 * }
	 * \endcode
	 * \param Subject_ the type of the subject to visit (note: this is
	 *        the only type the visitor will actually visit. Any nodes
	 *        of any different type will not be called upon to accept 
	 *        the visitor)
	 * \param TraversalPolicy_ a policy class that determines how the
	 *        the next node to visit is determined - i.e. how to traverse
	 *        the hierarchy
	 * */
	template <
		typename Subject_,
		template < typename > TraversalPolicy_ = TraversalPolicy >
	class Visitor
	{
	public :
		typedef Subject_ Subject;
		
		//! Get the data transported by the visitor
		//! \returns the data transported by the visitor. The type of this data is determined by the SubjectTraits specialization
		SubjectTraits< Subject >::Data getData() const { return data_; }
		//! Set the data transported by the visitor
		//! \param data the data to be transported by the visitor.
		void setData( SubjectTraits< Subject >::Data data ) { data_ = data; }

		//! Visit a given node
		//! \param subject first node to visit
		void visit(SubjectTraits< Subject >::Base * subject)
		{ VisitorTraits< Visitor< Subject_, TraversalPolicy_ > >::visit(subject, visitor); }
	private :
		//! The data transported by the visitor
		/** The visitor can accumulate and/or transport data during 
		 * its visit. This field holds that data and the getData and
		 * setData methods allow access to it. */
		SubjectTraits< Subject >::Data data_;
	};
}

#endif

