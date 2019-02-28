#!/bin/bash
echo "rm *.log *.txt?"
echo "Press 'y' to delete runs related files"
read choice
case "$choice" in
  y) rm *.log *.txt
  ;;
  *) echo "Exit without deleting"
  ;;
esac
