#!/bin/bash

for file in inputs/* ; do
  output="outputs/${file##*/}.BWT.out"
  alphabet="alphabets/DNA_alphabet.txt"
  echo "$output"
  ./trie "$file" $alphabet > "$output"
done