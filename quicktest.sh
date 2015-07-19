#!/bin/bash

if [ $# != 4 ]
then
    echo "Usage: quciktest.sh number side length ratio"
else
    time ./JxrEncApp -L $3 -i database/$3/$2/$1_$2_$2_r$3 -h $2 -w $2 -o out.jxr -R $4
    stat out.jxr
    rm out.jxr
fi
