#!/usr/bin/perl
#
# build_include_table.pl
# MegaMol Core
#
# Copyright (C) 2015 by Sebastian Grottel
# Alle Rechte vorbehalten. All rights reserved
#
use strict;
use File::Find;

my $inc_path = "../../include/mmcore";
my $outFile = "include_table.inc";

open (my $out, ">$outFile") || die "Cannot write output file \"$outFile\"\n";

print $out qq<#
# $outFile
# Copyright (C) 2015 by Sebastian Grottel
# Alle Rechte vorbehalten. All rights reserved
#
# Generated File! Do not edit!
#
# Generated by "build_include_table.pl"
#

sub get_mmcore_includes { 
	return (
>;

sub add_include_file {
	return unless -f;
	my $dir = $File::Find::dir;
	$dir =~ s|$inc_path||;
	$dir =~ s|^/||;
	if ($dir ne "") {
		$dir .= '/';
	}
	print $out "        '$dir$_',\n";
}

find(\&add_include_file, $inc_path);

print $out qq<    );
};

1;
>;

close ($out);
exit 0;
