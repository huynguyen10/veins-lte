#!/bin/perl

use List::Util qw(shuffle);

if($#ARGV != 1) {
	die("argument?")
}

my $command = "./run $ARGV[0] -u Cmdenv -c";

my @configs = `./run -a $ARGV[0]`;
my @runs = ();

foreach (@configs) {
	if ($_ =~ /Config ([^\:]*): (\d*)/) {
		for (my $i=0; $i < $2; $i++) {
			$n=$1;
			if ($1 =~ /^$ARGV[1]/) { push(@runs,". $command $n -r $i\n");}
		}
	}
}

print $_ foreach(shuffle @runs);
