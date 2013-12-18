harmonicCluster
===============

A estimate method of polyphonic music  
This program will split spectrum after lpc analyze.  

install:  
(you require gmake)  
make  

usage:  
harmonicCluster [option] [wav file]  
-l [frame length]  
-p [frame shift]  
-d [lpc order]  
-n [frame number]  
-h [number of harmonic ]  
-k [number of kernel]  

example:  
  harmonicCluster.exe -l 320 -p 320 -d 16 -n 15 -h 2 -k 1 > result.txt  
  gnuplot  
  >plot "result.txt"
  
