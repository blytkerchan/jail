-- MySQL dump 9.09
--
-- Host: localhost    Database: jail
-- ------------------------------------------------------
-- Server version	4.0.16

--
-- Table structure for table `questions`
--

DROP TABLE IF EXISTS answers;
CREATE TABLE answers (
  uid int(16) NOT NULL default '0',
  question_id int(16) NOT NULL,
  answer VARCHAR (160) default NULL,
  must_be_unique int(1) NOT NULL default '0',
  PRIMARY KEY  (uid)
) TYPE=MyISAM;

--
-- Dumping data for table `questions`
--

INSERT INTO answers VALUES (0,'1','This is an answer','0');
INSERT INTO answers VALUES (1,'1','This is another answer','0');
