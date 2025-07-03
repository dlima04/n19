#!/usr/bin/env bash

find . -type f \( -name "*.cpp" -o -name "*.hpp" \) | while read -r file; do
  if [ -s "$file" ]; then
    last_char=$(tail -c 1 "$file")
    if [ "$last_char" != "" ] && [ "$last_char" != $'\n' ]; then
      echo "$file"
    fi
  fi
done
