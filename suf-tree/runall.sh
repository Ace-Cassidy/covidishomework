#!/bin/bash

FLAG="--bwt"
TIME="/usr/bin/time"

for file in data/english-inputs/* ; do
  alphabet="data/alphabets/English_alphabet.txt"
  output="data/outputs/${file##*/}.BWT.out"
  stats="data/outputs/${file##*/}.BWT.stats"

  echo "$output"
  
  ./a.out "$file" $alphabet $FLAG > "$output" &&
  ($TIME -v ./a.out "$file" $alphabet) &> "$stats" 
done

for file in data/dna-inputs/* ; do
  alphabet="data/alphabets/DNA_alphabet.txt"
  output="data/outputs/${file##*/}.BWT.out"
  stats="data/outputs/${file##*/}.BWT.stats"
  
  echo "$output"
  
  ./a.out "$file" $alphabet $FLAG > "$output" &&
  ($TIME -v ./a.out "$file" $alphabet) &> "$stats" 
done