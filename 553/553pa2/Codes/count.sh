#!/usr/bin/env bash
for file in $1/*.txt; do
    mv "$file" "$1/$(basename "$file" .txt).cfg"
done