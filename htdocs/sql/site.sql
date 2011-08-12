-- MySQL dump 9.09
--
-- Host: localhost    Database: jail
-- ------------------------------------------------------
-- Server version	4.0.16

--
-- Table structure for table `bookmarks`
--

CREATE TABLE bookmarks (
  uid int(10) unsigned NOT NULL auto_increment,
  section varchar(20) default NULL,
  page varchar(20) default NULL,
  tag varchar(20) default NULL,
  label varchar(20) NOT NULL default '',
  user int(11) NOT NULL default '0',
  PRIMARY KEY  (uid)
) TYPE=MyISAM;

--
-- Dumping data for table `bookmarks`
--


--
-- Table structure for table `comments`
--

CREATE TABLE comments (
  uid int(10) unsigned NOT NULL auto_increment,
  user int(10) unsigned NOT NULL default '0',
  section varchar(50) NOT NULL default '',
  page varchar(50) NOT NULL default '',
  title varchar(50) NOT NULL default '',
  comment text NOT NULL,
  date date default NULL,
  PRIMARY KEY  (uid)
) TYPE=MyISAM;

--
-- Dumping data for table `comments`
--

INSERT INTO comments VALUES (1,1,'General','welcome','Truly an interesting project!','After working on this project for  number of months, I still find it truly interesting not only to define a new programming language for \\\"the masses\\\", but also (and perhaps above all) to create new software that will eventually implement this new language \\\"for the masses\\\".\r\n\r\nSo far, I have - for example - created a container library with non-blocking containers, which is a concept that was entirely new to me when I started on it; created a memory management library based on SMR, which is also something I had never done before, etc.\r\nAlso, it\\\'s been a very long time since I last did any assembly language programming (the days of DOS, to be exact). I now find uses for it that I hadn\\\'t thought of before, and thus find myself learning more fun stuff!\r\n\r\nActually, I can only say one thing for this project: \\\"Coolness abound!!\\\"','2004-05-28');
INSERT INTO comments VALUES (2,1,'General','goals','Portability','As far as the portability of the project\\\'s code is concerned, we may need a bit of help, here: all the code is (or rather: should be) POSIX-compliant but as far as architecture-dependant code goes, I have only implemented AI-32 (x86) code. The problem is that I don\\\'t have any machines that have a CPU that are not 32-bit Intel processors, so if I would write any code for, say, a Sparc machine, there would be no way fo me to test it.\r\n\r\nAnother problem is Windows: I haven\\\'t tested any of this on a Windows machine yet, and I\\\'m pretty sure it won\\\'t compile with any of Microsoft\\\'s compilers - or any other non-GNU compiler, for that matter. The problem, again, is in the achitecture-dependant code which uses GNU\\\'s inline assembly syntax and AT&T\\\'s assembly dialect.\r\n\r\nAll of these problems should be rather trivial to fix: Windows has system calls for (nearly?) every one of the atomic actions I\\\'ve written architecure-dependant code for, and the code for other platforms exists in every BSD kernel and could just be reaped there (we can\\\'t take Linux code because it is GPL\\\'d and the Jail code is double-licensed). The problem is in the testing, mostly.\r\n\r\nIf you have any programming knowledge, now would be a great time to step forward and be heard - mail to jail-ust-devel@lists.sourceforge.net or to ronald@landheer.com if you\\\'re interested in helping out.\r\n\r\nrlc','2004-05-28');
INSERT INTO comments VALUES (3,1,'General','how_you_can_help','Assembly programmers wanted!','I need someone who has a Sparc machine available to write some archtecture-dependant code for Sparc. The functions to implement, to the best of my knowledge, all correspond to existing Sparc-asm instructions so it should be really trivial to do..\r\n\r\nThe functions to be implemented are the atomic operations used by the different components of Jail: compare and swap (CAS), atomic increment, decrement and set.\r\n\r\nThe same would be nice for PowerPC (though PPC doesn\\\'t have a CAS instruction, it can be emulated) and Motorola chipsets. Itanium and x86-64 code would be nice as well.\r\n\r\nrlc','2004-05-28');

--
-- Table structure for table `sessions`
--

CREATE TABLE sessions (
  uid int(10) unsigned NOT NULL default '0',
  user int(10) unsigned NOT NULL default '0',
  IP varchar(16) NOT NULL default '',
  browser varchar(255) default NULL,
  expire int(11) NOT NULL default '0',
  PRIMARY KEY  (uid),
  KEY user (user)
) TYPE=MyISAM;

--
-- Table structure for table `users`
--

CREATE TABLE users (
  uid int(10) unsigned NOT NULL auto_increment,
  name varchar(50) NOT NULL default '',
  email varchar(50) default NULL,
  passwd varchar(50) NOT NULL default '',
  expire int(11) NOT NULL default '0',
  PRIMARY KEY  (uid),
  UNIQUE KEY name_2 (name),
  FULLTEXT KEY name (name,email)
) TYPE=MyISAM;

--
-- Dumping data for table `users`
--

INSERT INTO users VALUES (1,'ronald','ronald@landheer.com','2e7ad18453403cd5',1119108364);

