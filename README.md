harmonicCluster
===============

A estimate method of polyphonic music
This program will split spectrum after lpc analyze.

install:

(you require gmake)
make


usage:
harmonicCluster [option] [wav file]<br>
-l [frame length]<br>
-p [frame shift]<br>
-d [lpc order]<br>
-n [frame number]<br>
-h [number of harmonic ]<br>
-k [number of kernel]<br>

example:  
  harmonicCluster.exe -l 320 -p 320 -d 16 -n 15 -h 2 -k 1 > result.txt
  gnuplot
  >plot "result.txt"
  
