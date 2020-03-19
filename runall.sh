#!/bin/bash


for file in english-inputs/* ; do
  output="outputs/${file##*/}.BWT.out"
  stats="outputs/${file##*/}.BWT.stats"
  alphabet="alphabets/English_alphabet.txt"
  echo "$output"
  ("$(which time)" -v ./trie "$file" $alphabet > "$output") &> "$stats" 
done

for file in dna-inputs/* ; do
  output="outputs/${file##*/}.BWT.out"
  stats="outputs/${file##*/}.BWT.stats"
  alphabet="alphabets/DNA_alphabet.txt"
  echo "$output"
  ("$(which time)" -v ./trie "$file" $alphabet > "$output") &> "$stats" 
done
