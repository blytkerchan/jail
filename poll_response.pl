#! /usr/bin/perl -w
use Data::Dumper;
use DBI();

$print_header = 1;
$base_url     = '/cgi-bin/poll.pl';
$db_database  = 'jail';
$db_host      = 'localhost';
$db_username  = 'ronald';
$db_password  = '770217104';
$PROGID       = '$Id: poll_response.pl,v 1.2 2004/01/07 17:27:59 blytkerchan Exp $';

sub connect_database()
{
	$db{dsn} = "DBI:mysql:database=$db_database;host=$db_host";
	$db{dbh} = DBI->connect($db{dsn}, $db_username, $db_password, 
	                        {'RaiseError' => 1});
}

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

sub put_response($)
{
	my $sth;
	my $template;
	
	$sth = $db{dbh}->prepare("CREATE TABLE IF NOT EXIST responses (uid INT(16) NOT NULL DEFAULT '0' AUTO_INCREMENT PRIMARY KEY, question_id INT(16) NOT NULL DEFAULT '0', response TEXT NOT NULL);");
	$sth->execute();

	$template = "INSERT INTO responses VALUES (\'1\',\'%s\')";
	$sth->execute(sprintf($template, $_[0]));
}

sub output_body()
{
}

connect_database();
@response = <>;
put_response("@response");
output_header();
output_body();
output_footer();
