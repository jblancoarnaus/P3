#!/bin/bash

# Put here the program (maybe with path)
#mkdir 4kHz if its not created already
sample_rate=4000
for fwav in 4kHz/*.wav; do
    ff0=${fwav/.wav/.f0ref}
    echo $ff0
    cp $ff0 4kHz/
    f0name=$(echo "$ff0" | cut -f 3 -d '/')
    fname=$(echo "$fwav" | cut -f 3 -d '/')
    echo sox $fwav -r $sample_rate 4kHz/$fname 
    sox $fwav -r $sample_rate 4kHz/$fname || (echo "Error in $fwav $"; exit 1)
done

exit 0