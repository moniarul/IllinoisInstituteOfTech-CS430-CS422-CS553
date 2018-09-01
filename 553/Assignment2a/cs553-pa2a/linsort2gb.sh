#!/bin/bash
touch /exports/home/schatterjee/cs553-pa2a/linsort2gb.log
start_time="$(date -u +%s)"
LC_ALL=C sort /input/data-2GB.in > /tmp/linsort2gb.out
end_time="$(date -u +%s)"
valsort /tmp/linsort2gb.out 2>> /exports/home/schatterjee/cs553-pa2a/linsort2gb.log
elapsed="$(($end_time-$start_time))"
echo "Time Taken $elapsed seconds" 2>> /exports/home/schatterjee/cs553-pa2a/linsort2gb.log
