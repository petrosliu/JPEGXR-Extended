#!/bin/bash
rm out.jxr
rm out.raw
rm timetmp


list=`ls /home/petros/Desktop/database/Snapshots/32_*_589_236.raw`
#list=`ls /home/petros/Desktop/database/TraceData/32_*.raw`
#list=( '/home/petros/Desktop/database/TraceData/32_18_*.raw')
#list=( '/home/petros/Desktop/database/Snapshots/32_1_10_589_236.raw')
#list=( '/home/petros/Desktop/database/Snapshots/32_29_120_589_236.raw')

if [ "$#" -eq 0 ]; then
	qarray=( 1 5 10 20 50 100 150 200 255)
else
	qarray=( $1)
fi

for qp in "${qarray[@]}"
#for qp in {1..255}
do
count=0

for fname in $list
do

count=`expr $count + 1`

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

{ time ./JxrEncAppOri -L $bit -i $fname -h $height -w $width -o out.jxr -q $qp; } 2> timetmp
rt=$(cat timetmp | grep 'real' | awk '{print $2}')
rm timetmp
min=${rt%m*}
rt=${rt#*m}
sec=${rt%s*}
rt=$(echo "scale=4;$min*60+$sec"| bc)
./JxrDecAppOri -i out.jxr -o out.raw
snr=$(./SNR -c out.raw -o $fname -w $width -h $height -b $bit -C SNR)
rho=$(./SNR -c out.raw -o $fname -w $width -h $height -b $bit -C RHO)
sig=$(./SNR -c out.raw -o $fname -w $width -h $height -b $bit -C SIGMA)
size=$(stat -c%s "out.jxr")
raw=$(echo "$width*$height*32/8"| bc)
ratio=$(echo "scale=3;$raw/$size"| bc)
echo -e "$snr\t$qp\t$rho\t$sig\t$ratio\t$rt\t$count\t32\t$index\t$frame\t$width\t$height"

done

done
