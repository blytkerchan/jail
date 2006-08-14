/** \mainpage An implementation of the Visitor pattern
 * I recently read an article in IEEE Computer about the Visitor pattern and
 * "Componentization" ("Componentization: The Visitor Example", B. Meyer & 
 * K. Arnout, IEEE Computer, July 2006 (Vol. 39, No. 7) pp. 23-30). I found
 * their argument, that design patterns make for reusable patterns but not
 * reusable software components, compelling and the example of the Visitor
 * pattern very interesting, as a Visitor is definitely something you'd need
 * to visit a parse tree written in C++.
 * 
 * I checked Andrei Alexandrescu's solution from Modern C++ Design
 * (Addison-Wesley, 2001) and found it oddly lacked flexibility...
 * 
 * The implementation presented here doesn't require much of anything: it does
 * require a fairly up-to-date C++ compiler, but it actually doesn't require
 * any changes to the code that uses this implementation. For example: you can
 * send a Visitor into anything that can be loosely bound into a hierarchy: 
 * all you need to do is define the proper TraversalPolicy (to know how to 
 * traverse the hierarchy) SubjectTraits (to know its base class, how it's
 * coupled to a node in a hierarchy and how it will accept the Visitor) and
 * VisitorTraits (to know how to visit a subject). Normally, you don't need
 * to change the VisitorTraits (you'd just have to do that if you want to
 * support a radically different way of traversing a hierarchy, such as 
 * visiting it "both ways", visiting each node twice, etc.)
 *
 * You can find out how to use the rest of this library, specializing each of
 * the traits and (possibly) policies to suite your needs.
 *
 * \author Ronald Landheer-Cieslak */

//! The library's main namespace
/** All of the code related to this implementation of the Visitor pattern
 * can be found in this namespace. */
namespace Visitors
{ /* only here for documentation purposes */ }
