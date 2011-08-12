#! /usr/bin/perl
use CGI::Cookie;
use Data::Dumper;

require "poll_db.pl";
require "poll_http.pl";

$PROGID = '$Id: poll_result.pl,v 1.1 2004/04/08 15:13:28 blytkerchan Exp $';

$print_header = 1;

sub output_body()
{
	my $sth;
	my $template;
	my $ref;
	my %responsehash;
	my @commentarray;
	my %counthash;
	my %answerhash;
	my @keys;

	$template = "SELECT * FROM responses WHERE question_id=\'%d\'";
	$sth = $db{dbh}->prepare(sprintf($template, $question{uid}));
	$sth->execute();
	while ($ref = $sth->fetchrow_hashref())
	{
			  $responsehash{$$ref{remote_ip}} = $$ref{answers} if (exists $$ref{answers});
			  push @commentarray, $$ref{comment} if ($$ref{comment});
	}
	foreach (keys %responsehash)
	{
		while (1)
		{
			if ($responsehash{$_} =~ /([^\s]*)\s*(.*)/)
			{
				last unless($1 ne "");
				$counthash{$1} = $counthash{$1} + 1;
				$responsehash{$_} = $2;
			}
		}
	}
	$template = "SELECT * FROM answers WHERE uid=\'%d\'";
	@keys = sort {
		$counthash{$b} cmp $counthash{$a}
	} keys %counthash;       # and by value
	foreach (@keys)
	{
		$sth = $db{dbh}->prepare(sprintf($template, $_));
		$sth->execute();
		while ($ref = $sth->fetchrow_hashref())
		{
			$answerhash{$_} = $$ref{answer};
		}
	}
	print "<table>\n" if (@keys);
	print "<tr id=\"result_table_title\"><td>answer</td><td>frequency</td></tr>\n" if (@keys);
	foreach (@keys)
	{
		printf "<tr><td>%s</td><td>%d</td></tr>\n", $answerhash{$_}, $counthash{$_};
	}
	print "</table>\n" if (@keys);
}

connect_database();
%question = fetch_question();

output_header();
output_body();
output_footer();
