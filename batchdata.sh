#!/bin/bash
# This is my first script.





list=`ls /home/petros/Desktop/database/SIPI/32`

for fname in $list
do

index=${fname%%_*}
tmp=${fname#*_}
width=${tmp%%_*}
tmp=${tmp#*_}
height=${tmp%%_*}

./JxrEncApp -L 32 -i /home/petros/Desktop/database/SIPI/32/$fname -h $height -w $width -o out.jxr -q 1

done