#!/usr/bin/perl
# vi:set ai sw=4 ts=4 et smarttab:

# Try:    ./try.1.pl | convert - foo.png && eog foo.png

use strict;
use warnings;

my $red = 0;
my $green = 0;
my $blue = 0;

my $ticks = shift || 1000;
my $width = shift || 300;

my $state = 1;
my $sleep;

sub tick()
{
    # Fade to blue
    if ($state == 1) {
        if ($blue < 255) {
            $blue++, $green--, $red--;
        }
        else {
            $state = 2;
        }
    }

    # Fade to green
    if ($state == 2) {
        if ($green < 255) {
            $green++, $blue--;
        }
        else {
            if (rand(100) > 75) {
                $state++;
            }
            else {
                $state--;
            }
        }
    }

    # White intensity
    if ($state == 3) {
        $blue = 255;
        $red = 255;
        $green = 255;
        $sleep = 10;
        # axon_fire()
        $state++;
    }

    # Pause
    if ($state == 4) {
        if ($sleep-- == 0) {
            $state++;
        }
    }

    # Drown out gren and blue
    if ($state == 5) {
        if ($green) {
            $green--, $blue--;
            if ($state % 3 < 2) {
                $red--;
            }
        }
        else {
            $state++;
        }
    }

    if ($state == 6) {
        if ($red > 128) {
            $red--;
        }
        else {
            $state = 1;
        }
    }



    if ($red   < 0)    { $red    =   0; }
    if ($green < 0)    { $green  =   0; }
    if ($blue  < 0)    { $blue   =   0; }

    if ($red   > 255)  { $red    = 255; }
    if ($green > 255)  { $green  = 255; }
    if ($blue  > 255)  { $blue   = 255; }

    for my $i (1..$width) {
        printf "   %3d %3d %3d", $red, $green, $blue;
    }
    print "\n";
    print STDERR "$state    $red  $green  $blue\n";
}


print "P3 $width $ticks 255\n";

while ($ticks--) {
    tick()
}
