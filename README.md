# CPTS471 PA3

---

## Build

To build this project run either ```./mk.sh``` or ```./mk_04.sh```. Building
with ```./mk.sh``` will build task1 and task2 with debugging symbols and no
optimizations. Building with ```./mk_04.sh``` will build task1 and task2 with
no debugging symbols and with all compiler optimizations turned on.

## Usage

The ```mk.sh``` build script will output a ```t1``` and ```t2``` file.

The ```mk_04.sh``` script will output a ```t1_opt``` and ```t2_opt``` file.

Invoke these programs as
```./<program> data/alphabets/DNA_alphabet.txt data/dna-inputs/*.fasta```

## Notes

Note that ```data/dna-inputs/MERS_2012_KF600620.fasta.ignore``` is not picked up
by the ```*.fasta``` glob star, this file is excluded due to its containing
ambiguous reads. We felt that it was easiest to simply exclude this file from our
analysis, rather than substituting arbitrary base pairs into the sequence.
