#! /usr/bin/perl
use CGI::Cookie;
use Data::Dumper;

require "poll_db.pl";
require "poll_http.pl";

$PROGID = '$Id: poll_result.pl,v 1.1 2004/01/11 11:42:46 blytkerchan Exp $';

$print_header = 1;

sub output_body()
{
}

connect_database();
%question = fetch_question();

output_header();
output_body();
output_footer();
