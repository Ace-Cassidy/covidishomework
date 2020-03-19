#!/bin/bash


for file in english-inputs/* ; do
  output="outputs/${file##*/}.BWT.out"
  alphabet="alphabets/English_alphabet.txt"
  echo "$output"
  ./trie "$file" $alphabet > "$output"
done

for file in dna-inputs/* ; do
  output="outputs/${file##*/}.BWT.out"
  alphabet="alphabets/DNA_alphabet.txt"
  echo "$output"
  ./trie "$file" $alphabet > "$output"
done
