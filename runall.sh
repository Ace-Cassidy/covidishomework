#!/bin/bash

TIMEFORMAT='Command timed: %C\nElapsed Time (wallclock): %e\nPeak memory usage (kbytes): %M\nExit status: %x\n'

for file in english-inputs/* ; do
  output="outputs/${file##*/}.BWT.out"
  stats="outputs/${file##*/}.BWT.stats"
  alphabet="alphabets/English_alphabet.txt"
  echo "$output"
  ("$(which time)" -f "$TIMEFORMAT" ./trie "$file" $alphabet > "$output") &> "$stats" 
done

for file in dna-inputs/* ; do
  output="outputs/${file##*/}.BWT.out"
  stats="outputs/${file##*/}.BWT.stats"
  alphabet="alphabets/DNA_alphabet.txt"
  echo "$output"
  ("$(which time)" -f "$TIMEFORMAT" ./trie "$file" $alphabet > "$output") &> "$stats" 
done

for file in ref-outputs/* ; do
  bname=${file##*/}
  echo diff "$file" "outputs/${bname}"
done