#!/bin/bash

if [ "$#" -ne 2 ]
then
    echo "Usage: ./prepare.sh <runs_file> <config_name>"
    exit 1
else
    runs_file=$1
    conf=$2
fi

perl generateRunsFile_ini.pl omnetpp.ini ${conf} > ${runs_file}


 
