#!/bin/bash

if [ $# != 5 ]
then
    echo "Usage: quciktest.sh number side length R/r/q ratio/qp"
else
    time ./JxrEncApp -L $3 -i database/$3/$2/$1_$2_$2_r$3 -h $2 -w $2 -o out.jxr -$4 $5
    stat out.jxr
    rm out.jxr
fi
