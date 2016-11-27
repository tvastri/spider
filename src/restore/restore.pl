#!/usr/bin/perl -w

use strict;
use File::Path qw(make_path remove_tree);
use File::Basename;
use File::Copy;

my @words = ();
my $char0;
my $char1;
my $char2;
my $char3;
my $path;
my $dir;

open (IDXFILE, "<", "/home/dpandey/.spider/index.txt") or die $!;

while(<IDXFILE>)
{
    chomp $_;
    @words = split(/\s+/, $_);
    $char0 = substr($words[1], 0, 1);
    $char1 = substr($words[1], 1, 1);
    $char2 = substr($words[1], 2, 1);
    $char3 = substr($words[1], 3, 1);
    #print $char0, " ", $char1, " ", $char2, " ", $char3, " ", $words[1], "\n";
    $path = "/disk2/BACKUP/$char0/$char1/$char2/$char3/$words[1]";
    print $path, "\n";
    print dirname($words[0]), "    ", basename($words[0]), "\n";
    # Create directories
    if (-e dirname($words[0]))
    {
        printf "Directory " . dirname($words[0]) . "exists\n";
    }
    else
    {
        make_path(dirname($words[0])) or die "make_path failed: $!";
    }
    # Copy file   
    copy($path, $words[0]) or die "Copy failed: $!";
    chmod(oct($words[2]), $words[0]) or die "chmod failed: $!";
    utime($words[3], $words[4], $words[0]) or die "utime failed: $!";
}

close IDXFILE;
