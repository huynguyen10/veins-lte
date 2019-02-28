#!/bin/bash

if [ "$#" -ne 3 ]
then
  echo "Usage: ./genPerlRunFile.sh <config_name> <para_file> <perlRun_file>"
  exit 1
else
  config_name=$1
  para_file=$2
  perlRun_file=$3
fi

readarray -t lines < ${para_file}
numPara=${#lines[@]}
combinations=("")

for (( i = 0; i < ${numPara}; i++ )); do
    temp=()
    for j in ${!combinations[@]}; do
        for val in ${lines[$i]}; do
            combi="${combinations[$j]} ${val}"
            temp+=("${combi}")
        done
    done
    combinations=("${temp[@]}")
    
    
done

# Open file descriptor (fd) 3 for read/write on a text file.
exec 3> $perlRun_file
for n in ${!combinations[*]}; do
    echo ". sh extractData.sh ${config_name}${combinations[$n]}" >> ${perlRun_file}
done
# Close fd 3
exec 3>&-

echo "Number of combinations: ${#combinations[@]}"
echo "Done."