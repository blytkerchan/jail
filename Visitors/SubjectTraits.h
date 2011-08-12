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
#ifndef _visitors_subjecttraits_h
#define _visitors_subjecttraits_h

namespace Visitors
{
	//! Traits of a subject to be visited
	/** This class defines how to interact with a subject to be visited
	 * by the Visitor. By default, the subject's base class is the class
	 * itself, the node to which it is linked is the class itself as
	 * well and the data the visitor contains for it is a simple integer
	 * (because we can't store a void). This is probably not what you want -
	 * especially if you want the non-intrusive option of having the
	 * acceptance of a visitor done by something other than the subject, so
	 * you can specialize this class to your heart's content, as long as 
	 * you provide the same typedefs and methods. 
	 * \param Subject the type of the subject
	 * \param Visitor the type of the visitor */
	template < typename Subject, typename Visitor >
	struct SubjectTraits
	{
		//! Type of the data stored in the visitor
		typedef int Data;
		//! Type of the base class of all subjects
		typedef Subject Base;
		//! Type of the hierarchy nodes that link the subjects
		typedef Subject Node;

		/** \brief Called when the visitor wants to be accepted by the subject
		 * \param subject the subject to visit
		 * \param visitor the visitor to be accepted */
		static void accept( Subject * subject, Visitor * visitor ) { subject->accept(visitor); }
		/** \brief get the node corresponding to the given subject base
		 * In order to traverse the hierarchy, the visitor needs to
		 * get the nodes that link the subjects together. This method
		 * get ssuch a node from the base class of a subject.
		 * \param base base to get the node from 
		 * \returns a pointer to the node */
		static Node * getNode(Base * base) { return base; }
		/** \brief get the subject corresponding to a node
		 * Once the next node to visit has been found, the Visitor
		 * needs to have the subject that belongs to that node in
		 * order to visit it
		 * \param node the node to get the subject from
		 * \returns the subject */
		static Base * getSubject(Node * node) { return node; }
	};
}

#endif

