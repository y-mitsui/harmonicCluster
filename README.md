harmonicCluster
===============

An estimate method of polyphonic music  
This program split spectrum by the harmonic cluster after LPC analyzing.  

##Install
(You require gmake and gcc.)  
>make  

##Usage
harmonicCluster [option] [wav file]  
-l [frame length]  
-p [frame shift]  
-d [lpc order]  
-n [frame number]  
-h [number of harmonic ]  
-k [number of classter]  

##Example
>harmonicCluster.exe -l 320 -p 320 -d 16 -n 15 -h 2 -k 1 > result.txt  
>gnuplot  
>plot "result.txt"
  
