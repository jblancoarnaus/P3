#!/bin/bash

# Put here the program (maybe with path)
GETF0="get_pitch"
EVAL="pitch_evaluate"

lower_index_bound=1                     #lower bound of the for loop
upper_index_bound=100                   #upper bound of the for loop
offset=0                                #offset of the variable we want to iterate
div=1000                               #controls the step size of the variable when we iterate it (=2 -> /2)

for index in $(seq $lower_index_bound $upper_index_bound); do #vary $index from (lower_index_bound) to (upper_index_bound)
    k0=$(bc <<<"scale=5; $offset+$index/$div")    #stores offset+$index/div in k0 (uses basic calculator (bc) since shell doesn't seem to support floating point by default)


    for fwav in pitch_db/train/*.wav; do
     ff0=${fwav/.wav/.f0}
    $GETF0 $fwav $ff0 -c $k0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
    done
    res=`$EVAL -l pitch_db/train/*f0ref || (echo "Error in $GETF0 $fwav $ff0"; exit 1)`
    echo "$k0 $res"
done

exit 0