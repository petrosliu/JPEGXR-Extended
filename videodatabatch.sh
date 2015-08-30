#!/bin/bash

list=`ls /home/petros/Desktop/database/Snapshots`

for fname in $list
do

tmp=${fname%%.*}
bit=${tmp%%_*}
tmp=${tmp#*_}
index=${tmp%%_*}
tmp=${tmp#*_}
frame=${tmp%%_*}
tmp=${tmp#*_}
width=${tmp%%_*}
tmp=${tmp#*_}
height=${tmp%%_*}

#for i in {5..15}
#do
echo $fname
./JxrEncApp -L $bit -i /home/petros/Desktop/database/Snapshots/$fname -h $height -w $width -o out.jxr -R 10 -A

rm out.jxr

#done
done
