#!/bin/bash

# Put here the program (maybe with path)
GETF0="get_pitch"

lower_index_bound=0                     #lower bound of the for loop
upper_index_bound=100                   #upper bound of the for loop
offset=0                                #offset of the variable we want to iterate
div=1                                   #controls the step size of the variable when we iterate it (=2 -> /2)

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"

    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done





for index in $(seq $lower_index_bound $upper_index_bound); do #vary $index from (lower_index_bound) to (upper_index_bound)
    echo "i = $index"
    k0=$(bc <<<"scale=2; $offset+$index/$div")    #stores offset+$index/div in k0 (uses basic calculator (bc) since shell doesn't seem to support floating point by default)
    echo "k0 = $k0"                     #display k0 value 
    echo -n "$k0 ">> out_alpha.txt      #print k0 value in out_alpha.txt

    for filewav in $DB/*/*wav; do

        if [[ ! -f $filewav ]]; then
            echo "Wav file not found: $filewav" >&2
            exit 1
        fi

        filevad=${filewav/.wav/.vad}
        
        $CMD -i $filewav -o $filevad -t $k0 || exit 1

        # Alternatively, uncomment to create output wave files
        #    filewavOut=${filewav/.wav/.vad.wav}
        #    $CMD $filewav $filevad $filewavOut || exit 1

    done
    #vad_evaulation_printless.pl is almost like vad_evaluation.pl, except that most prints on display have been delated to view the TOTAL results properly
    #this .pl is also in charge of printing the "TOTAL" results into out.alpha.txt, since parsing it from the screen through the shell script seemed way too complex
    scripts/vad_evaluation_printless.pl $DB/*/*lab 
    echo ""
done


exit 0