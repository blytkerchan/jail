-- MySQL dump 9.09
--
-- Host: localhost    Database: jail
-- ------------------------------------------------------
-- Server version	4.0.16

--
-- Table structure for table `questions`
--

DROP TABLE IF EXISTS questions;
CREATE TABLE questions (
  uid int(16) NOT NULL default '0',
  question varchar(120) NOT NULL,
  examples int(1) NOT NULL default '0',
  answers int(1) NOT NULL default '0',
  commentable int(1) NOT NULL default '0',
  PRIMARY KEY  (uid)
) TYPE=MyISAM;

--
-- Dumping data for table `questions`
--

INSERT INTO questions VALUES (0,'What\'s in a namespace?','0','0','1');
INSERT INTO questions VALUES (1,'A question with an example','1','1','0');
