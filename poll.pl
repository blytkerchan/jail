#! /usr/bin/perl
use Data::Dumper;
use DBI();

$PROGID = '$Id: poll.pl,v 1.10 2004/01/08 17:39:12 blytkerchan Exp $';

$print_header = 1;
$response_url = '/cgi-bin/poll_response.pl';
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

sub output_header()
{
	print "Content-Type: text/html;\n" if ($print_header);
	print "\n" if ($print_header);
	print "<html>\n<!-- Generated by $PROGID -->\n";
	print "<link rel=stylesheet href=\"/poll.css\" type=\"text/css\"/>\n";
	print "<body>\n";
}

sub output_footer()
{
	print "</body>\n";
	print "</html>\n";
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

	return %retval;
}

sub output_body()
{
	my %question;
	my $arrayref;
	my $i;
	my $template;
	
	%question = fetch_question();

	print "<form action=\"$response_url\" method=\"post\" enctype=\"text/plain\" accept-charset=\"utf8\" style=\"width: 200;\">\n";
	print "<div id=\"question\">".$question{question}."</div>\n";

	$arrayref = $question{examples};
	$i = 1;
	foreach (@$arrayref)
	{
		print "<div id=\"example\">\n";
		print "<div id=\"example_title\">Example $i:</div>";
		print "$_\n";
		print "</div>\n";
		$i++;
	}
	if ($question{answers})
	{
		$arrayref = $question{answers};
		if ($question{unique_answer})
		{
			$template = "<input type=\"radio\" name=\"answer\" value=\"%s\"/>%s<br/>\n";
		}
		else
		{
			$template = "<input type=\"checkbox\" name=\"answer\" value=\"%s\"/>%s<br/>\n";
		}
		foreach (@$arrayref)
		{
			printf($template, $_, $_);
		}
	}
	print "<div id=\"comment\"><textarea name=\"comment\" style=\"text-align: left; width: 100%; height: 300;\">Your comment here please</textarea></div>\n" if ($question{commentable});
	print "<div style=\"text-align: right;\"><input type=\"reset\" value=\"Reset\"/><input type=\"submit\" value=\"Submit\"/></div>\n";
	print "</form>\n";
}

connect_database();
output_header();
output_body();
output_footer();
