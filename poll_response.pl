#! /usr/bin/perl -w
use Data::Dumper;
require "poll_db.pl";
require "poll_http.pl";

$print_header = 1;
$PROGID       = '$Id: poll_response.pl,v 1.5 2004/01/11 11:42:46 blytkerchan Exp $';

sub output_body()
{
	print "<META HTTP-EQUIV=\"Refresh\" CONTENT=\"30; poll_result.pl\">\n";
	print "<h1>Thank you!</h1>\n";
	print "<p>Thank you for answering our poll.<br/>\n";
	print "Your response has been recorded.<br/>\n";
	print "You will be redirected to the results page in 30 seconds. Should this not work, please click <a href=\"poll_result.pl\">here</a>.</p>\n";
}

sub add_to_hash(%$$)
{
	my @array;
	my $hashref = $_[0];
	my $key = $_[1];
	my $value = $_[2];

	if (exists $$hashref{$key})
	{
		$arrayref = $$hashref{$key}; 
		@array = @$arrayref;
	}
	push @array, "$value\n";
	$$hashref{$key} = \@array;
}

while (<>)
{
	if ($last_key ne 'comment')
	{
		if (/\s*([^\s=]*)\s*?=(.*)/)
		{
			$last_key = $1;
			SWITCH1: {
				if ($last_key eq 'question_id') { $response{question_id} = $2; 	last SWITCH1;	}
				if ($last_key eq 'comment') 	{ $response{comment} .= $2; 	last SWITCH1;	}
				add_to_hash(\%response, $1, $2);
			}
		}
	}
	else
	{
		$response{comment} .= $_;
	}
}

connect_database();
put_response(\%response);

output_header();
output_body();
output_footer();

