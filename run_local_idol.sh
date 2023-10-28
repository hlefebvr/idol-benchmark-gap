#!/bin/bash

ARGS=(
  "external"
  "bab true"
  "bab false"
  "bap true 0 false true"
  "bap true 0 false false"
  "bap true 0 true true"
  "bap true .3 true true"
)

FOLDERS=(
  "n4"
  #"n3"
  #"n2"
)

for FOLDER in "${FOLDERS[@]}"
do
  for ARG in "${ARGS[@]}"
  do
    ./run_idol.sh ./GAP/data/$FOLDER $ARG || exit
  done
done
