#!/usr/bin/perl
# vi:set ai sw=4 ts=4 et smarttab:

# Try:    ./fade.pl | convert - foo.png && eog foo.png

use strict;
use warnings;

my $length = shift || 2000;
my $width = shift || 200;
print "P3 $width $length 255\n";

my @colors = (
   #[   0,   0,   0, "Black"   ],  
   #[ 255, 255, 255, "White"   ],  
    [ 255,   0,   0, "Red"     ],  
    [   0, 255,   0, "Lime"    ],  
    [   0,   0, 255, "Blue"    ],  
    [ 255, 255,   0, "Yellow"  ],  
    [   0, 255, 255, "Cyan"    ],  
    [ 255,   0, 255, "Magenta" ],  
   #[ 192, 192, 192, "Silver"  ],  
   #[ 128, 128, 128, "Gray"    ],  
   #[ 128,   0,   0, "Maroon"  ],  
   #[ 128, 128,   0, "Olive"   ],  
   #[   0, 128,   0, "Green"   ],  
   #[ 128,   0, 128, "Purple"  ],   # was: 186,85,211
    [ 175,   0, 175, "Purple"  ],  
   #[   0, 128, 128, "Teal"    ],  
   #[   0,   0, 128, "Navy"    ],  
);

my $r = 0;
my $g = 0;
my $b = 0;

my $lines;

while (1) {
    my $target = $colors[rand @colors];

    my $target_r = $target->[0];
    my $target_g = $target->[1];
    my $target_b = $target->[2];
    my $name = $target->[3];

    printf STDERR "%-10s   %3d %3d %3d\n", $name, $target_r, $target_g, $target_b;

    while ($target_r != $r || $target_g != $g || $target_b != $b) {
        if    ($target_r > $r) { $r++; }
        elsif ($target_r < $r) { $r--; }

        if    ($target_g > $g) { $g++; }
        elsif ($target_g < $g) { $g--; }

        if    ($target_b > $b) { $b++; }
        elsif ($target_b < $b) { $b--; }

        for my $i (1 .. $width) {
            printf "   %3d %3d %3d", $r, $g, $b;
        }
        print "\n";

        exit if $lines++ > $length;
    }
}
