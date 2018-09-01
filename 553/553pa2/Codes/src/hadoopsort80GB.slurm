#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=/exports/home/schatterjee/cs553-pa2b/hadoopsort80GB.log
#SBATCH --wait-all-nodes=1

touch /exports/home/schatterjee/cs553-pa2b/hadoopsort80GB.log
cd /exports/home/schatterjee/cs553-pa2b/
srun javac -cp $(hadoop classpath) HadoopSort.java

srun jar cf HadoopSort.jar HadoopSort*.class
srun hdfs dfs -rm -R /user/schatterjee/output
srun hadoop jar HadoopSort.jar HadoopSort /input/data-80GB/data-80GB.in /user/schatterjee/output