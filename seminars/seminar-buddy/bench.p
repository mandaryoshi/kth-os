set terminal png
set output "bench.png"

set title "3 graphs for benchmarking"

set key right center

set xlabel "amount of blocks"
set ylabel "amount of data"

set xrange [0:200]
set yrange [0:4000]

plot "4kblocks.dat" u 1:2 w linespoints title "4kblocks", \"allocated.dat" u 1:2 w linespoints title "allocated memory", "blocks.dat" u 1:2 w linespoints title "allocated blocks"
