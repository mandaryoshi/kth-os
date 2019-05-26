set terminal png
set output "graph.png"

set grid
set title "pthread vs green thread"

set key right bottom

set xlabel "Amount of loops"
set ylabel "Clock cycles"

set logscale x

plot "data.dat" using 1:2 pt 7 w linespoints title "green thread", "data.dat" using 1:3 pt 22  w linespoints title "pthread"
