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
  uid integer NOT NULL default '0',
  question varchar(120) NOT NULL,
  examples integer NOT NULL default '0',
  answers integer NOT NULL default '0',
  commentable integer NOT NULL default '0',
  ask_email integer NOT NULL default '0',
  one_answer integer NOT NULL default '1',
  PRIMARY KEY  (uid)
) TYPE=MyISAM;

--
-- Dumping data for table `questions`
--

INSERT INTO questions VALUES (0,'What\'s in a namespace?','0','0','1','0');
INSERT INTO questions VALUES (1,'A question with an example','1','1','1','0');

