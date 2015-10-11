#!/bin/bash
list=`ls /home/petros/Desktop/database/Snapshots/32*0_589_236.raw`

for fname in $list
do

tmp=${fname%%.*}
tmp=${tmp##*/}
bit=${tmp%%_*}
tmp=${tmp#*_}
index=${tmp%%_*}
tmp=${tmp#*_}
time=${tmp%%_*}
tmp=${tmp#*_}
width=${tmp%%_*}
tmp=${tmp#*_}
height=${tmp%%_*}

#for r in {5..15}
#do
r=10
echo $fname
./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -R $r

#done

done