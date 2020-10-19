#!/usr/bin/env bash
set -e

function fix_loader_path () {
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
}

fix_loader_path "/tmp/obsdeps" "@loader_path/../deps" "prebuild/libobs/bin/*.{dylib,so}"
fix_loader_path "/tmp/obsdeps" "@loader_path/../deps" "prebuild/libobs/obs-plugins/*.so"
fix_loader_path "/tmp/obsdeps/bin" "@loader_path" "prebuild/libobs/deps/bin/*.dylib"
fix_loader_path "/tmp/obsdeps/lib" "@loader_path/../lib" "prebuild/libobs/deps/bin/*.dylib"
fix_loader_path "/tmp/obsdeps/lib" "@loader_path" "prebuild/libobs/deps/lib/*.dylib"

# Fix libobs_node
install_name_tool -id "libobs_node.node" "prebuild/libobs_node.node"
install_name_tool -change "@rpath/libobs.0.dylib" "@loader_path/libobs/bin/libobs.0.dylib" "prebuild/libobs_node.node"