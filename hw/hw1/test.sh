#!/usr/bin/env bash

# comand line args: $1(test executable)

make clean
make 

counter = 1
while IFS=' ' read -r cache_size file comparison_output
do

  echo "- Running Test ${counter} -"
  valgrind ./$1 $cache_size $file > output.txt
  echo

  echo "- Results -"
  diff output.txt $comparison_output
  echo

  echo "removing output.txt..."
  rm output.txt
  echo

  counter = counter + 1

done < "test.txt"

echo "- All Tests Completed -"
