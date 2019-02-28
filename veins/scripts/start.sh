#!/bin/bash
if [ "$#" -ne 3 ]
then
  echo "Usage: ./start.sh <processes> <runs_file> <log_file>"
  exit 1
else
  num=$1
  runs_file=$2
  log_file=$3
fi
touch ${log_file}
python runmaker4.py -j $num -l ${log_file} -n 30 ${runs_file}
