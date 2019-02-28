#!/bin/bash
if [ "$#" -ne 3 ]
then
  echo "Usage: ./multiCoresRun.sh <processes> <run_file> <log_file>"
  exit 1
else
  num=$1
  runs_file=$2
  log_file=$3
fi

BASEDIR=$(dirname $0)
scripts=$BASEDIR"/../../scripts"
run_make=$scripts"/runmaker4.py"

echo "Run simulation with <num> processes..."
touch ${log_file}
python $run_make -j $num -l ${log_file} -n 30 ${runs_file}
