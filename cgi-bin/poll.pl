#! /usr/bin/perl
use Data::Dumper;
require "poll_db.pl";
require "poll_http.pl";

$PROGID = '$Id: poll.pl,v 1.1 2004/04/08 15:13:28 blytkerchan Exp $';

$print_header = 1;
$response_url = '/cgi-bin/poll_response.pl';

sub output_body()
{
	my %question;
	my $arrayref;
	my $i;
	my $template;
	
	%question = fetch_question();

	print "<form action=\"$response_url\" method=\"post\" enctype=\"text/plain\" accept-charset=\"utf8\" style=\"width: 200;\">\n";
	print "<input type=\"hidden\" name=\"question_id\" value=\"".$question{uid}."\"/>\n";
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
