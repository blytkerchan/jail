#! /usr/bin/perl
use DBI();

$PROGID = '$Id: poll_db.pl,v 1.1 2004/04/08 15:13:28 blytkerchan Exp $';

$db_database  = 'jail';
$db_host      = 'localhost';
$db_username  = 'ronald';
$db_password  = '770217104';

sub connect_database()
{
	$db{dsn} = "DBI:mysql:database=$db_database;host=$db_host";
	$db{dbh} = DBI->connect($db{dsn}, $db_username, $db_password, 
	                        {'RaiseError' => 1});
}

sub fetch_question()
{
	my $sth;
	my $question;
	my $val;
	my $question_id;
	my %retval;
	my @examples;
	my @answers;
	
	$sth = $db{dbh}->prepare("SELECT MAX(uid) FROM questions");
	$sth->execute();
	$ref = $sth->fetchrow_arrayref();
	$question_id = $$ref[0];

	$sth = $db{dbh}->prepare("SELECT * FROM questions WHERE uid=$question_id");
	$sth->execute();
	$question = $sth->fetchrow_hashref();
	$retval{question} = $$question{question};
	$retval{commentable} = $$question{commentable};
	if ($$question{examples})
	{
		$sth = $db{dbh}->prepare("SELECT * FROM examples WHERE question_id=\'$question_id\'");
		$sth->execute();
		while ($val = $sth->fetchrow_hashref())
		{
			push @examples, $$val{example};
		}
		$retval{examples} = \@examples;
	}
	if ($$question{answers})
	{
		$sth = $db{dbh}->prepare("SELECT * FROM answers WHERE question_id=\'$question_id\'");
		$sth->execute();
		while ($val = $sth->fetchrow_hashref())
		{
			push @answers, $$val{answer};
			$retval{unique_answer} = 1 if ($$val{must_be_unique});
		}
		$retval{answers} = \@answers;
	}
	$retval{uid} = $$question{uid};
	$retval{ask_email} = $$question{ask_email};
	$retval{one_answer} = $$question{one_answer};

	return %retval;
}

sub put_response(%)
{
	my $sth;
	my $answerref;
	my $arrayref;
	my $hashref;
	my @answer_array;
	my @answerid_array;
	my $template;
	my $responseref = $_[0];
	my $create_request =<<EOE
CREATE TABLE IF NOT EXISTS responses (
	uid int(11) NOT NULL auto_increment,
	question_id int(11) NOT NULL default '0',
	comment text,
	answers varchar(160) default NULL,
	remote_ip varchar(16) default NULL,
	log_time timestamp,
	PRIMARY KEY  (uid)
) TYPE=MyISAM
EOE
;	
	$sth = $db{dbh}->prepare($create_request);
	$sth->execute();

	$template = "SELECT * FROM answers WHERE question_id=\'%d\' AND answer like \'%s\';";
	$arrayref = $$responseref{answer};
	if ($arrayref)
	{
		@answer_array = @$arrayref;
		foreach(@answer_array)
		{
			chomp $_;
			chop $_;
			$sth = $db{dbh}->prepare(sprintf($template, $$responseref{question_id}, $_));
			$sth->execute();
			$hashref = $sth->fetchrow_hashref();
			push @answerid_array, $$hashref{uid};
		}
	}
	
	$template = "INSERT INTO responses (question_id, comment, answers, remote_ip) VALUES (\'%d\',\'%s\',\'%s\',\'%s\')";
	$sth = $db{dbh}->prepare(sprintf($template, $$responseref{question_id}, "$$responseref{comment}", "@answerid_array", $ENV{REMOTE_ADDR}));
	$sth->execute();
}

