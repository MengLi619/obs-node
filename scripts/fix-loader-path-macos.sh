#!/usr/bin/env bash

prefix=$1
replaced_prefix=$2
file_glob=$3
for lib in $(eval ls $file_glob); do
  echo "Fix loader path for '$lib'"
  for dep in $(otool -L "$lib" | grep -o "${prefix}\S*"); do
    if [[ $(basename "$lib") == $(basename "$dep") ]]; then
      new_name=$(basename "$lib")
      echo "Change file name $dep to $new_name"
      install_name_tool -id "$$new_name" "$lib"
    else
      replaced_dep=$(echo "$dep" | sed -e "s|${prefix}|${replaced_prefix}|")
      echo "Change dependence name '$dep' to '$replaced_dep'"
      install_name_tool -change "$dep" "$replaced_dep" "$lib"
    fi
  done
done