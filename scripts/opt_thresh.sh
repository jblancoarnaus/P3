#!/bin/bash

# Put here the program (maybe with path)
GETF0="get_pitch"
EVAL="pitch_evaluate"

#k0 loop parameters power threshold
lower_index_bound=1                     #lower bound of the for loop
upper_index_bound=100                   #upper bound of the for loop
offset=-30                                #offset of the variable we want to iterate
div=2                                  #controls the step size of the variable when we iterate it (=2 -> /2)

#k1 loop parameters
lower_index_bound1=15                    #lower bound of the for loop
upper_index_bound1=35                    #upper bound of the for loop
offset1=0                                #offset of the variable we want to iterate
div1=25                                  #controls the step size of the variable when we iterate it (=2 -> /2)

#k2 loop parameters
lower_index_bound2=0                     #lower bound of the for loop
upper_index_bound2=10                    #upper bound of the for loop
offset2=0                                #offset of the variable we want to iterate
div2=25                                  #controls the step size of the variable when we iterate it (=2 -> /2)

for index in $(seq $lower_index_bound $upper_index_bound); do #vary $index from (lower_index_bound) to (upper_index_bound)
    k0=$(bc <<<"scale=5; $offset+$index/$div")    #stores offset+$index/div in k0 

    for index1 in $(seq $lower_index_bound1 $upper_index_bound1); do
        k1=$(bc <<<"scale=5; $offset1+$index1/$div1")  #stores offset+$index/div in k1 
        for index2 in $(seq $lower_index_bound2 $upper_index_bound2); do
            k2=$(bc <<<"scale=5; $offset2+$index2/$div2")  #stores offset+$index/div in k1 
            for fwav in pitch_db/train/*.wav; do
                ff0=${fwav/.wav/.f0}
                $GETF0 $fwav $ff0 -0 $k0 -1 $k1 -2 $k2 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
            done
            res=`$EVAL -l pitch_db/train/*f0ref || (echo "Error in $GETF0 $fwav $ff0"; exit 1)`
            echo "$k0 $k1 $k2 $res"
        done
    done
done

exit 0