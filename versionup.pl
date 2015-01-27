#!/usr/bin/perl -w
use strict;

my $major = 0;
my $minor = 0;
my $revision = `git rev-list --count HEAD`;
my $build = 0;

$revision=~s/^\s+|\s+$//g;

open VERSION_H_READ, "<version.h";
if(fileno(VERSION_H_READ)) {
	while(my $row = <VERSION_H_READ>) {
		if($row=~m|MAJOR\s+=\s+(\d+);|) {
			$major = $1;
		}
		if($row=~m|MINOR\s+=\s+(\d+);|) {
			$minor = $1;
		}
		if($row=~m|BUILD\s+=\s+(\d+);|) {
			$build = $1;
		}
	}
}
close VERSION_H_READ;

foreach my $arg (@ARGV) {
	if($arg=~m|-get|i) {
		print "$major.$minor.$revision.$build";
		exit;
	}
}

$build++;

open VERSION_H_WRITE, ">version.h" or die $!;
print VERSION_H_WRITE <<EOF_VERSION_H;
#ifndef VERSION_H
#define VERSION_H
namespace Version
{
    static const int MAJOR = $major;
    static const int MINOR = $minor;
    static const int REVISION = $revision;
    static const int BUILD = $build;
}
#endif // VERSION_H
EOF_VERSION_H
close VERSION_H_WRITE;