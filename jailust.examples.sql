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
  question_id int(16) NOT NULL,
  example VARCHAR(16) default NULL,
  PRIMARY KEY  (uid)
) TYPE=MyISAM;

--
-- Dumping data for table `questions`
--

INSERT INTO questions VALUES (0,'1','One example of an example');
INSERT INTO questions VALUES (1,'1','Another example of an example');
