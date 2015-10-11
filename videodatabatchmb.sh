#!/bin/bash
if [ $# == 0 ]
then

rm out.jxr
rm out.raw

list=`ls /home/petros/Desktop/database/Snapshots/*00_589_236.raw`

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
#./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -R 10 -A -E

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -q 1 > mb$bit.txt

#./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -q 100

done
fi

if [ $# == 3 ]
then

fname="/home/petros/Desktop/database/Snapshots/${1}_${2}_${3}_589_236.raw"
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

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -R 10 -A -E

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -q 1

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -q 100

fi

if [ $# == 1 ]
then

fname="test_data${1}"
bit=32
width=576
height=560

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -R 10 -A -E

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -q 1

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -q 100


fi