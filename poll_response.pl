#! /usr/bin/perl -w
use Data::Dumper;

@response_array = <>;
print Dumper(\@response_array);
open OUTPUT, ">output";
print OUTPUT Dumper(\@response_array);

print "Content-Type: text/html;\n";
print "\n";
print "<html><body><p>Thanks</p></body></html>\n";

