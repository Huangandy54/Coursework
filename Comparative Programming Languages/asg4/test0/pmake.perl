#!/usr/bin/perl
# Andy Huang (ahuang44@ucsc.edu)
# Jackson Mccabe (jbmccabe@ucsc.edu)
#
# NAME
#    pmake — perl implementation of a subset of gmake
#
# SYNOPSIS
#    pmake [-d] [target]
#
# DESCRIPTION
#    The pmake utility executes a list of shell commands associated with each target,
#    typically to create or update files of the same name. The Makefile contains
#    entries that describe how to bring a target up to date with respect to those on
#    which it depends, which are called prerequisites
#    
#
use strict;
use warnings;
use POSIX qw(strftime);
use Getopt::Std;

#subroutine given in graph.perl
sub parse_dep ($) {
   my ($line) = @_;
   return undef unless $line =~ m/^(\S+)\s*:\s*(.*?)\s*$/;
   my ($target, $dependency) = ($1, $2);
   my @dependencies = split m/\s+/, $dependency;
   return $target, \@dependencies;
}
#declarations for Hashes and variables
my %targetHash;
my %commandHash;
my %macroHash;
my $currentTarget;
my %OPTIONS;
my $initTarget;
my $firstTargetFound=0;
getopts ("d", \%OPTIONS);
#open makefile
my $filename = 'Makefile.txt';
open my $infile, "<$filename"
    or die "Could not open file '$filename' $!";

sub convertMacro($){
    my ($macro)=@_;
    return $macroHash{$macro};
}
#subroutine to fill out target hashtables, command hashtables and macro hashtables
sub fillHash{
    #line num for debug
    my $lineNum=0;
    while(defined (my $line = <$infile>)) {
        #splits line by line
        chomp $line;
        #print "$lineNum "unless $line =~ m\^#\;
        $lineNum++;
        #if line does not start with #
        
        if (not $line=~ m\^#\) {
            #if line is not all spaces 
            if (not $line=~/^ *$/){
                #if line is a macro
                if ($line =~ m/(\S*)\s*=\s(.*)/){
                    my $temp1=$1;
                    my $temp2=$2;
                    while ($temp2 =~ m/\$\{([^}]+)}/) {
                        print "$lineNum\n";
                        my $converted=convertMacro($1);
                        $temp2 =~ s/\$\{[$1}]+}/$converted/g;
                        
                    }
                    $macroHash{$temp1}=$temp2;    
                    
                    
                }
                # if line is in the form of ___ : ____
                elsif ($line=~m/^(\S+)\s*:\s*(.*?)\s*$/) {
                    my ($target, $deps) = parse_dep $line;
                    print "$0: syntax error: $line\n" and next unless defined $target;
                    if ($firstTargetFound eq 0) {
                        $initTarget=$target;
                        $firstTargetFound=1;
                    }
                    $currentTarget=$target;
                    $targetHash{$target} = $deps;
                }
                #if line is a command
                elsif ($line =~ m/\t(.*)/) {
                    #print "true, $lineNum";
                    $commandHash{$currentTarget}=$1;
                }
            }
        }
        #print "$line\n" unless $line =~ m\^#\;
        
        
        #prints size of targetHash
        #my @target = keys %targetHash;
        #my $size = @target;
        #print "1 - Hash size:  is $size\n";

    }
}
sub recRun($){
    my ($target)=@_;
    if(not $targetHash{$target}){#if no prereq, run command
        system($commandHash{$target});
    }

    foreach my $prereq ($targetHash{$target}){#iterate through prereq list,   CHECK SYNTAX @$
        recRun($prereq) if not -e $prereq;#recursively compiles prereqs that DNE
       }#end while1

       #all prereqs should now exist

    foreach my $prereq ($targetHash{$target}){#iterate through prereq list again,   CHECK SYNTAX @$
           if(mtime($target) < mtime($prereq) || mtime($target)==undef){#if target is older than prereq or target is undef
               System( $commandHash{$target});#execute commands
               last;
           }
       }#end while2
}#end rec

fillHash;
#less than 0 means there was no target 
# $initTarget will be set to the first target found in fillHash.
if ($#ARGV lt 0) {
    #print("\n$initTarget\n");
    recRun($initTarget);
}#else we have a target
else{
    $initTarget=$ARGV[0];
    #print("\n$initTarget\n");
    recRun($initTarget);
}
=begin comment
#printing targethash debug
for my $target (keys %targetHash) {

    print "\"$target\"";
    my $deps = $targetHash{$target};
    if (not @$deps) {
       print " has no dependencies";
    }else {
       print " depends on";
       print " \"$_\"" for @$deps;
    }
    print "\n";
}
#printing commandHash
for my $target (keys %commandHash) {

    print "\"$target\"";
    my $command = $commandHash{$target};
    
    print "'s command is: ";
    print " \"$_\"" for $command;
    
    print "\n";
}
#printing macroHash
for my $variableName (keys %macroHash) {

    print "\"$variableName\"";
    my $definition = $macroHash{$variableName};
    
    print " = ";
    print " \"$_\"" for $definition;
    
    print "\n";
}
=end comment
=cut
