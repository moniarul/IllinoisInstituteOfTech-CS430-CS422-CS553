#!/bin/bash
touch /exports/home/schatterjee/cs553-pa2a/linsort20gb.log
start_time="$(date -u +%s)"
LC_ALL=C sort /input/data-20GB.in > /tmp/linsort20gb.out
end_time="$(date -u +%s)"
valsort /tmp/linsort20gb.out 2>> /exports/home/schatterjee/cs553-pa2a/linsort20gb.log
elapsed="$(($end_time-$start_time))"
echo "Time Taken $elapsed seconds" 2>> /exports/home/schatterjee/cs553-pa2a/linsort20gb.log
