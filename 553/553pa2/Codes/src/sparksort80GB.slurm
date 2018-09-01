#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=/exports/home/schatterjee/cs553-pa2b/sparksort80GB.log
#SBATCH --wait-all-nodes=1

touch /exports/home/schatterjee/cs553-pa2b/sparksort80GB.log
cd /exports/home/schatterjee/cs553-pa2b/

srun hdfs dfs -rm -R /user/schatterjee/output
srun javac -cp /opt/spark-2.3.0-bin-hadoop2.7/jars/spark-core_2.11-2.3.0.jar:/opt/spark-2.3.0-bin-hadoop2.7/jars/scala-compiler-2.11.8.jar:/opt/spark-2.3.0-bin-hadoop2.7/jars/scala-library-2.11.8.jar  SparkSort.java
srun jar cvf SparkSort.jar SparkSort.class
srun spark-submit --class SparkSort --master yarn --deploy-mode client --driver-memory 1g --executor-memory 1g --executor-cores 1 --num-executors 1 SparkSort.jar /input/data-80GB /user/schatterjee/output-spark