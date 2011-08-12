DROP TABLE IF EXISTS news;
CREATE TABLE `news` (
  `uid` INT NOT NULL AUTO_INCREMENT, 
  `start_date` DATE NOT NULL, 
  `end_date` DATE NOT NULL, 
  `title` VARCHAR(30) NOT NULL, 
  `content` TEXT NOT NULL,
  `author_id` INT NOT NULL, 
  PRIMARY KEY (`uid`),
  INDEX (`start_date`, `end_date`),
  FULLTEXT (`title`, `content`)
);
--
-- Dumping data for table `questions`
--
INSERT INTO `news` (`start_date`, `end_date`, `title`, `content`) VALUES ('2004-08-23', '2004-10-01', 'Build system make-over', 'The build system is going through a complete make-over using a non-recursive generated Makefile. Building is now much faster and no longer requires multiple configure scripts to be run. However, we do require a recent version of GNU Make. IMHO seeing as GNU Make is very portable and free, that is acceptable.', '1');
INSERT INTO `news` (`start_date`, `end_date`, `title`, `content`) VALUES ('2004-08-23', '2004-10-01', 'Status of development', 'Three core libraries are taking shape: <ul><li>libthread<br/>The threading support library. Different versions of an rw-lock are under development and will be tested. A clone-based implementaion of thread creation would be nice, so we would no longer need the underlying pthread implementation for anything.</li><li>libmemory<br/>The memory management library. There is one out-standing bug in the SMR implementation which still needs to be fixed and we do not have a memory pool implementation yet - which would be nice.</li><li>libcontain<br/>the container library now features a complete set of abstract data types, most of which are non-blocking, lock-free, thread-safe. Still to be added are <ul><li>A binary tree implementation<br/>on which the map should be based (in stead of the binomial tree on which it is based now)<li><li>A complete list-based hash implementation<br/>Development of the implementation has started but was stalled while other issues took precedence.</li></ul></li></ul>All these and more are currently under development - patches and code reviews are looked upon very kindly - discussions should take place on the jail-usr-devel list (also available via Gmane).', '1');

