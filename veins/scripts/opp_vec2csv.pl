#!/usr/bin/env perl
#
# opp_vec2csv.pl -- converts OMNeT++ .vec files to csv format
#
# (Refer to POD sections at end of file for documentation)
#
# Copyright (C) 2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
use strict;
use warnings;
use Getopt::Long qw(:config no_ignore_case bundling auto_version auto_help);
use Pod::Usage;

$main::VERSION = 3.02;

my $verbose = 0;
my %vectorNames = ();
my %attrNames = ();
my $sampleRate = 1;
my $randomSeed = -1;
GetOptions (
	"filter|F:s" => \%vectorNames,
	"attr|A:s" => \%attrNames,
	"sample|s:i" => \$sampleRate,
	"seed|S:i" => \$randomSeed,
	"verbose|v" => \$verbose
) or pod2usage("$0: Bad command line options.\n");
pod2usage("$0: No vector files given.\n") if (@ARGV < 1);

if ($randomSeed == -1) {
	$randomSeed = srand() ^ time();
	print STDERR "using dynamic random seed $randomSeed\n" if $verbose;
}
else {
	print STDERR "using static random seed $randomSeed\n" if $verbose;
}

# get list of file names from command line
my @fileNames;
while (my $fileName = shift @ARGV) {
	push (@fileNames, $fileName);
}

# print header
print "event";
print "\t"."time";
print "\t"."node";
foreach my $attrName (sort keys %attrNames) {
	my $value = $attrNames{$attrName};
	$value = $attrName unless (defined $value and $value ne "");
	print "\t".$value;
}
foreach my $vectorName (sort keys %vectorNames) {
	my $value = $vectorNames{$vectorName};
	$value = $vectorName unless (defined $value and $value ne "");
	print "\t".$value;
}
print "\n";

# iterate over vector files
foreach my $fileName (@fileNames) {

	# this is where we store all the data
	my %events = ();

	print STDERR "reading \"".$fileName."\"...\n" if $verbose;

	my $FILE;
	open($FILE, $fileName) or die("Error opening file.");

	my $fileSize = -s $fileName;

	print STDERR "reading header...\n" if $verbose;

	# read header
	my %attrValues = ();
	while (<$FILE>) {

		# found blank line
		if (m{
				^
				\r?\n$
				}x) {
			last;
		}

		# found attr
		if (m{
				^attr
				\s+
				(?<attr>[^ ]+)
				\s+
				(?<value>.+)
				\r?\n$
				}x) {
			$attrValues{$+{attr}} = $+{value};
			next;
		}
	}

	print STDERR "reading body...\n" if $verbose;

	# read body
	my @nodName = (); # vector_id <-> nod_name mappings
	my @vecName = (); # vector_id <-> vec_name mappings
	my @vecType = (); # vector_id <-> type mappings
	while (<$FILE>) {

		# found vector data
		if (m{
				^
				(?<vecnum>[0-9]+)
				\s+
				(?<event>[0-9]+)
				\s+
				(?<vecdata>.*)
				\r?\n$
			}x) {

			my $event = $+{event};

			# obey sampling
			if ($sampleRate > 1) {
				srand($randomSeed + $event);
				next unless int(rand($sampleRate)) == 0;
			}

			# print progress
			if ($verbose and ($. % 10000 == 0)) {
				print STDERR sprintf("%.1f", tell($FILE)/1024/1024)."M/".sprintf("%.1f", $fileSize/1024/1024)."M (".sprintf("%.1f", tell($FILE)/$fileSize*100)."%)\r";
			}

			# look up definition
			my $nod_name = $nodName[$+{vecnum}];
			my $vec_name = $vecName[$+{vecnum}];
			my $vec_type = $vecType[$+{vecnum}];

			# vec_name must have been given on command line
			next unless exists($vectorNames{$vec_name});

			# extract time and value
			my $time = "";
			my $value = "";
			if ($vec_type eq 'ETV') {
				unless ($+{vecdata} =~ m{
							^
							(?<time>[0-9e.+-]+)  # allow -1.234e+56
							\s+
							(?<value>[0-9e.+-]+)  # allow -1.234e+56
							$
							}x) {
					print STDERR "cannot parse as ETV: \"".$+{vecdata}."\"\n";
					next;
				}
				$time = $+{time};
				$value = $+{value};
			} else {
				print STDERR "unknown vector type: \"".$vec_type."\"\n";
				next;
			}

			$events{$event."|".$nod_name}{"__TIME"} = $time;
			$events{$event."|".$nod_name}{"__EVENT"} = $event;
			$events{$event."|".$nod_name}{"__NODE"} = $nod_name;
			$events{$event."|".$nod_name}{$vec_name} = $value;


			next;
		}

		# found vector definition
		if (m{
				^vector
				\s+
				(?<vecnum>[0-9.]+)
				\s+
				(("(?<nodname1>[^"]+)")|(?<nodname2>[^\s]+))
				\s+
				(("(?<vecname1>[^"]+)")|(?<vecname2>[^\s]+))
				(\s+(?<vectype>[ETV]+))?
				\r?\n$
				}x) {
			$nodName[$+{vecnum}] = defined($+{nodname1})?$+{nodname1}:"" . defined($+{nodname2})?$+{nodname2}:"";
			$vecName[$+{vecnum}] = defined($+{vecname1})?$+{vecname1}:"" . defined($+{vecname2})?$+{vecname2}:"";
			$vecType[$+{vecnum}] = defined($+{vectype})?$+{vectype}:"";
			next;
		}

	}

	close($FILE);

	print STDERR "done processing                             \r" if $verbose;

	# print body
	foreach my $eventAndNode (sort { $a cmp $b } keys %events) {
		my $time = $events{$eventAndNode}{"__TIME"};
		my $event = $events{$eventAndNode}{"__EVENT"};
		my $node = $events{$eventAndNode}{"__NODE"};
		print $event;
		print "\t".$time;
		print "\t".$node;
		foreach my $attrName (sort keys %attrNames) {
			my $value = $attrValues{$attrName};
			print "\t".(defined $value ? $value : "");
		}
		foreach my $vectorName (sort keys %vectorNames) {
			my $value = $events{$event."|".$node}{$vectorName};
			print "\t".(defined $value ? $value : "");
		}
		print "\n";
	}

	print STDERR "done                                        \n" if $verbose;
}

__END__

=head1 NAME

opp_vec2csv.pl -- converts OMNeT++ .vec files to csv format

=head1 SYNOPSIS

opp_vec2csv.pl [options] [file ...]

-F --filter [name=alias]

	add a column for vector [name], calling it [alias] (if provided)

-A --attr [name=alias]

	add a column for attribute [name], calling it [alias] (if provided)

-s --sample [rate]

	output only a random sample of one in [rate] observations

-S --seed [seed]

	use a specific random seed for pseudo-random sampling

-v --verbose

	log debug information to stderr

e.g.: opp_vec2csv.pl -A configname -F senderName -F receivedBytes=bytes input.vec >output.csv

=cut

