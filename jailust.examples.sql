-- MySQL dump 9.09
--
-- Host: localhost    Database: jail
-- ------------------------------------------------------
-- Server version	4.0.16

--
-- Table structure for table `questions`
--

DROP TABLE IF EXISTS examples;
CREATE TABLE examples (
  uid integer NOT NULL default '0',
  question_id integer NOT NULL,
  example TEXT default NULL,
  PRIMARY KEY  (uid)
) TYPE=MyISAM;

--
-- Dumping data for table `questions`
--

INSERT INTO examples VALUES (0,'1','One example of an example');
INSERT INTO examples VALUES (1,'1','Another example of an example');
