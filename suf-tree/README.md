# CPTS471 Programming Assignment 2

## Team

Ace Cassidy

Miranda Stoddard

## Usage

trie is a program which builds a suffix tree from a given input file and alphabet

after building, invoke the trie program as:
```
trie <input fasta> <alphabet file> [flag]
```

### Build

compile as ```gcc -g *.c -o trie```, or if using vscode ```CTRL+SHIFT+b```

### Debug

debug with gdb, of if using vscode a launch.json is included

### Options

* noflag = create the tree with no output
* --bwt = create the tree and output its newline seperated BWT
* --stats = create tree and report tree stats (node count etc..)
* --dfs = create tree and print each node via depth first search
* --post = create tree and print each node via post order traversal