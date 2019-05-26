set terminal png
set output "tlb.png"

set title "TLB benchmark, 4 KiByes pages, 10 Gi operations"

set key right center

set xlabel "number of pages"

set ylabel "time in s"

#use log scale if we use doubling of number of pages set logscale x 2

plot "tlb.dat" u 1:2 w linespoints title "page refs"
