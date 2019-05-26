set terminal pngcairo dashed
set output "dots.png"
set termoption dash

set for [i=1:5] linetype i dt i
set grid
set title "pthread vs green thread"

set key right bottom

set xlabel "Amount of loops"
set ylabel "Clock cycles"

set logscale x

show style line

plot "data.dat" using 1:2 lt 1 lc 3 title "green thread", "data.dat" using 1:3 lt 3 lc 3 title "pthread"
