<h1>The Jail-Ust Container Library</h1>
<p>The libcontain library provides a large and growing number of fast and thread-safe containter objects written in C and C++</p>
<p>The 0.2 release introduces  number of important new features including a heap and a map, both based on a lock-free binomial tree. It also includes the required memory management to make the containters impervious to the ABA problem and thus truly thread-safe.</p>
<p>The libcontain library now provides the following container types:
<ul>
<li>a series of hash tables</br>
libcontain includes the hash table implementation from GNU glib and a hash implementation in C++ it inherited from the libhash library it originates from. This latter has been rendered thread-safe in the 0.2 release and implements a linear hashing algorithm. A single hash table implementation that will replace them both is on the drawing boards and is intended to be thread-safe and non-blocking.</li>
<li>an array (vector) implementation<br>
libcontain currently includes a non-blocking array implementation. However, this implementation presents a race-condition in concurrent calls to any of its writing functions if the array is in the process of being resized. There is currently no known non-blocking way to solve this race condition. Help is welcome in this area.</li>
<li>a binomial tree<br>
The binomial tree provided by libcontain does not provide any algorithmic logic in its implementation: it is a generic container that can be used as the basis for other container implementations, such as a heap or a map (as a matter of fact, the heap and the map are both based on this binomial tree)</li>
<li>a heap<br>
libcontain provides an automatically sorting heap implementation that (obviously) uses a heap sort to sort the elements it contains. Unfortunately, the heap implementation is not non-blocking (although bases on a non-blocking binomial tree implementation). It contains four levels of locks: a reader count and a write lock at the level of the nodes, and a reader count and a write lock at the level of the container. The locks are posed on the container for as short a time as possible so at to prevent the progress of the algorithm to block too long. If anyone knows of a non-blocking heap algorithm, input is more than welcome.</li>
<li>a list<br>
libcontain provides a non-blocking list implementation based on an algorithm by M.M. Micheal (also the inventor of Safe Memory Reclamation on which libmemory is based).</li>
<li>a map<br>
The map implementation included in libcontain is based on the binomial tree implementation. Unlike some implementations based on binary trees, this implementation stores values in each node of the tree (not just the leaf nodes) and therefore allows a more memory-efficient way to store information than hashes and other map implementations. Also, the map implementation does not use a "less" function but uses a three-way comparison to compare keys (much like the helper function used by qsort). The implementation is almost (but not quite) non-blocking: for most intents and purposes, there are no real locks: read and write operations may be re-directed along a branch of the binomial tree if a node that they traverse is being deleted, but will not block for that. However, a remove operation will wait for another remove operation to finish if it traverses a node being removed.</li>
<li>a queue<br>
a non-blocking queue implementation is provided</li>
<li>a stack<br>
a non-blocking stack implementation is also provided</li>
</ul>
</p>
<p>In future versions, the hash implementation will be replaced by a non-blocking one and the remaining problems with the existing containers will (hopefully) be solved. Help is also needed to write the architecture-dependant code for non-IA32 platforms such as Sparc.</p>
