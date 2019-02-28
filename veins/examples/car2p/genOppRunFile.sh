#!/bin/bash
if [ "$#" -ne 2 ]
then
  echo "Usage: ./genOppRunFile.sh <config_name> <oppRun_file>"
  exit 1
else
  conf=$1
  oppRun_file=$2
fi

BASEDIR=$(dirname $0)
scripts=$BASEDIR"/../../scripts"
prepare=$scripts"/generateRunsFile_ini.pl"

echo "Generate <oppRun-file>..."
perl $prepare omnetpp.ini ${conf} > ${oppRun_file}

echo "Done."
