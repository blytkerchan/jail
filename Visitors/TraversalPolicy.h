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
#ifndef _visitors_traversalpolicy_h
#define _visitors_traversalpolicy_h

namespace Visitors
{
	//! Traversal policy for a DOM-like hierarchy
	/** This class is here more as an example than anything else. It provides
	 * a means of traversing a DOM-like hierarchy of nodes in which the
	 * subject of the visit is linked to a node that has a getFirstChild,
	 * getNextSibling and a getParent accessor.
	 * \param Subject_ the type of the subject */
	template < typename Subject_ >
	class TraversalPolicy
	{
	public :
		typedef typename SubjectTraits< Subject_ >::Base SubjectBase;
		typedef typename SubjectTraits< Subject_ >::Node SubjectNode;
	
		//! Called to find out which is the next subject to visit
		/** The Visitor calls this method to find out which is the next 
		 * subject to visit. It should return 0 if the visit is over
		 * after the current node
		 * \param base current subject
		 * \returns the next subject to visit or 0 if none */
		static SubjectBase * getNext(SubjectBase * base)
		{
			SubjectNode * node( SubjectTraits< Subject_ >::getNode(base) );
			SubjectNode * retval(node->getFirstChild());
			if (!retval)
			{
				retval = node->getNextSibling();
				if (!retval)
				{
					SubjectNode * parent(node->getParent());
					while (!retval && parent)
					{
						retval = parent->getNextSibling();
						parent = parent->getParent();
					}
				}
				else
				{ /* yup! */ }
			}
			else
			{ /* we have a winner! */ }
			if (retval)
				return SubjectTraits< Subject_ >::getSubject( retval );
			else return 0;
		}
	};
}

#endif

