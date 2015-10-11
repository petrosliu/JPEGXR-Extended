#!/bin/bash
if [ $# == 0 ]
then

rm out.jxr
rm out.raw

list=`ls /home/petros/Desktop/database/Snapshots/*0_589_236.raw`

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

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -R 10 -A -E
./JxrDecApp -i out.jxr -o out.raw
snr1=$(./PSNR out.raw $fname $width $height $bit)
size1=$(stat -c%s "out.jxr")
raw=$(echo "$width*$height*$bit/8"| bc)
target=$(echo "$raw/10"| bc)
ratio=$(echo "scale=4;$raw/$size1"| bc)


./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -R 10
./JxrDecApp -i out.jxr -o out.raw
snr2=$(./PSNR out.raw $fname $width $height $bit)
size2=$(stat -c%s "out.jxr")
#echo $snr1 $snr2
d1=$(echo "$snr2 - $snr1"| bc)
d2=$(echo "$target - $size1"| bc)
echo -e "$bit\t$index\t$frame\t$snr1\t$snr2\t$d1\t$size1\t$size2\t$target\t$d2\t$ratio"

done
fi

if [ $# -ge 3 ]
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
./JxrDecApp -i out.jxr -o out.raw
snr1=$(./PSNR out.raw $fname $width $height $bit)
size1=$(stat -c%s "out.jxr")
raw=$(echo "$width*$height*$bit/8"| bc)
target=$(echo "$raw/10"| bc)
ratio=$(echo "scale=4;$raw/$size1"| bc)
if [ $# == 4 ]
then
	./PSNR out.raw $fname $width $height $bit $4
fi

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -R 10
./JxrDecApp -i out.jxr -o out.raw
snr2=$(./PSNR out.raw $fname $width $height $bit)
size2=$(stat -c%s "out.jxr")

if [ $# == 4 ]
then
	./PSNR out.raw $fname $width $height $bit $4
fi

#echo $snr1 $snr2
d1=$(echo "$snr2 - $snr1"| bc)
d2=$(echo "$target - $size1"| bc)
echo -e "$bit\t$index\t$frame\t$snr1\t$snr2\t$d1\t$size1\t$size2\t$target\t$d2\t$ratio"

fi

if [ $# == 1 ]
then

fname="test_data${1}"
bit=32
width=576
height=560

./JxrEncApp -L $bit -i /home/petros/Dropbox/JXR/jxrlib/$fname -h $height -w $width -o out.jxr -R 10 -A -E
./JxrDecApp -i out.jxr -o out.raw
snr1=$(./PSNR out.raw $fname $width $height $bit)
size1=$(stat -c%s "out.jxr")
raw=$(echo "$width*$height*$bit/8"| bc)
target=$(echo "$raw/10"| bc)
ratio=$(echo "scale=4;$raw/$size1"| bc)


./JxrEncApp -L $bit -i /home/petros/Dropbox/JXR/jxrlib/$fname -h $height -w $width -o out.jxr -R 10
./JxrDecApp -i out.jxr -o out.raw
snr2=$(./PSNR out.raw $fname $width $height $bit)
size2=$(stat -c%s "out.jxr")
#echo $snr1 $snr2
d1=$(echo "$snr2 - $snr1"| bc)
d2=$(echo "$target - $size1"| bc)
echo -e "$bit\t$fname\t$snr1\t$snr2\t$d1\t$size1\t$size2\t$target\t$d2\t$ratio"

fi