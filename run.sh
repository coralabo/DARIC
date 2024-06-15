ulimit -c unlimited

#!/bin/bash
op=$1
if [ "$op" == "start" ]; then


   ./rf_tcad --dfg_file=data/test.txt  --II=2 --pea_column=4 --pea_row=4 --childNum=12 

  
 
  
  
  
 
 
elif [ "$op" == "clear" ]; then
  rm -rf log/*
else
  echo "./run (start | clear)"
fi
