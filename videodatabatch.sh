#!/bin/bash

list=`ls /home/petros/Desktop/database/Snapshots/32*30_589_236.raw`
rm rho.txt
for fname in $list
do

tmp=${fname%%.*}
tmp=${tmp##*/}
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
echo $fname >> rho.txt
./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -q 1 >> rho.txt

rm out.jxr

done


let n=0

while read line
do
if [ "$n" = "0" ]
then
	fname=$(echo "${line}"| awk '{print $1}')
	tmp=${fname%%.*}
	tmp=${tmp##*/}
	bit=${tmp%%_*}
	tmp=${tmp#*_}
	index=${tmp%%_*}
	tmp=${tmp#*_}
	frame=${tmp%%_*}
	tmp=${tmp#*_}
	width=${tmp%%_*}
	tmp=${tmp#*_}
	height=${tmp%%_*}
else
	qp=$(echo "${line}"| awk '{print $1}')
	./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -q $qp > /dev/null
	echo $(stat -c%s "out.jxr")
fi

let n=$n+1
if [ "$n" = "7" ]
then
let n=0
fi
done < rho.txt