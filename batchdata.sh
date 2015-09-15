#!/bin/bash
# This is my first script.

list=`ls /home/petros/Desktop/database/SIPI/8`

for fname in $list
do

index=${fname%%_*}
tmp=${fname#*_}
width=${tmp%%_*}
tmp=${tmp#*_}
height=${tmp%%_*}

for i in {1..19}
do
for k in {0..1}
do

let j=$k*5
r=$i.$j
./JxrEncApp -L 8 -i /home/petros/Desktop/database/SIPI/8/$fname -h $height -w $width -o out.jxr -R $r

done
done

done



list=`ls /home/petros/Desktop/database/SIPI/16`

for fname in $list
do

index=${fname%%_*}
tmp=${fname#*_}
width=${tmp%%_*}
tmp=${tmp#*_}
height=${tmp%%_*}

for i in {1..19}
do
for k in {0..1}
do

let j=$k*5
r=$i.$j
./JxrEncApp -L 16 -i /home/petros/Desktop/database/SIPI/16/$fname -h $height -w $width -o out.jxr -R $r

done
done

done



list=`ls /home/petros/Desktop/database/SIPI/32`

for fname in $list
do

index=${fname%%_*}
tmp=${fname#*_}
width=${tmp%%_*}
tmp=${tmp#*_}
height=${tmp%%_*}

for i in {1..19}
do
for k in {0..1}
do

let j=$k*5
r=$i.$j
./JxrEncApp -L 32 -i /home/petros/Desktop/database/SIPI/32/$fname -h $height -w $width -o out.jxr -R $r

done
done

done