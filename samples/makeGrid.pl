#!/bin/perl

print '<?xml version = "1.0" standalone = "no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 20000802//EN" 
  "http://www.w3.org/TR/2000/CR-SVG-20000802/DTD/svg-20000802.dtd">
<svg width="400" height="400" style="fill:grey; stroke:none; stroke-width:0;">
';

foreach $i (0..100) {
  $x = $i * 4;
  foreach $j (0..100) {
    $y = $j * 4;
    print " <rect width='3' height='3' x='$x' y='$y'/>\n";
  }
}
print "</svg>\n";
