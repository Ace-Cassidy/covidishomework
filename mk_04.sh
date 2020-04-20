#!/bin/bash

gcc task1.c utility/*.c  suf-tree/*.c align-table/*.c -O4 -o t1_opt

gcc task2.c utility/*.c  suf-tree/*.c align-table/*.c -O4 -o t2_opt
