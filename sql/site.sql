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
  PRIMARY KEY  (uid)
) TYPE=MyISAM;

--
-- Dumping data for table `comments`
--


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
-- Dumping data for table `sessions`
--


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

INSERT INTO users VALUES (1,'ronald','ronald@landheer.com','2e7ad18453403cd5',1119034751);

