#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=/exports/home/schatterjee/cs553-pa2a/mysort2GB.log
#SBATCH --wait-all-nodes=1

touch /exports/home/schatterjee/cs553-pa2a/mysort2GB.log
cd /exports/home/schatterjee/cs553-pa2a/
srun javac Mysort2GB.java
srun java -Xmx7168m Mysort2GB
srun valsort /tmp/op2GB
