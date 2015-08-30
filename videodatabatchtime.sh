#!/bin/bash

list=`ls /home/petros/Desktop/database/Snapshots`

for fname in $list
do

tmp=${fname%%.*}
bit=${tmp%%_*}
tmp=${tmp#*_}
index=${tmp%%_*}
tmp=${tmp#*_}
time=${tmp%%_*}
tmp=${tmp#*_}
width=${tmp%%_*}
tmp=${tmp#*_}
height=${tmp%%_*}

qp=$(./JxrEncApp -L $bit -i /home/petros/Desktop/database/Snapshots/$fname -h $height -w $width -o out.jxr -R 10)

time ./JxrEncApp -L $bit -i /home/petros/Desktop/database/Snapshots/$fname -h $height -w $width -o out.jxr -R 10 -A

time ./JxrEncApp -L $bit -i /home/petros/Desktop/database/Snapshots/$fname -h $height -w $width -o out.jxr -q $qp


done
