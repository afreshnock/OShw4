#!/bin/sh
#sbatch --mem-per-cpu=512M --ntasks-per-node=16 --nodes=1 --constraint=moles ./pt1shell.sh
#gcc -pthread -o pt1 pt1New.c
./pt1