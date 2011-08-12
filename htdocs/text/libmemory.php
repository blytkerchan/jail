<h1>The Jail-Ust Memory Management Library</h1>
<p>The libmemory library provides an implementation of M.M. Micheal's Safe Memory Reclamation algorithm in C.</p>
<p>The Safe Memory Reclamation algorithm provides a convenient way to get rid of the so-called ABA problem in which a value changes from A to B back to A between reads. If, for example, the value you're reading is a pointer and the memory to which it points is freed and re-allocated between two reads, what you do with the memory it points to in that time may result in mayhem. The SMR algorithm prevents this by registering a "hazardous reference" to then pointer thus preventing the memory from being reclaimed while a reference exists.</p>
<p>The implementation is designed to fully implement the SMR algorithm without restricting the user to provide such information as the maximum number of threads used by the application or the batch size used by the algorithm. It will run on any POSIX-compliant platform and uses the POSIX threads API for thread-local storage. It should, however, be a trivial effort to port this library to Windows.</p>
