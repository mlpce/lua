#!/usr/bin/env bash
set -e

# This script converts the source files to GEMDOS compatible 8.3 filenames.
# It then creates a zip file, adding the files with LF converted to CRLF.

# Make 8.3 directory structure
rm -Rfv 8.3/LUA*
mkdir -p 8.3/LUA/BUILD/TOS/LATTICEC
mkdir -p 8.3/LUA/INPUT
mkdir -p 8.3/LUA/LUAC

function copy_lower_upper() {
  local wild_path=$1
  for path in $wild_path
  do
    local destination_path=$(echo $path | tr [:lower:] [:upper:])
    cp -v $path 8.3/LUA/BUILD/TOS/$destination_path
  done
}

# Copy files as 8.3 filenames
copy_lower_upper "latticec/lua.prj"
copy_lower_upper "latticec/lua54.prj"
copy_lower_upper "latticec/stackval.c"
copy_lower_upper "../../input/input.[ch]"
copy_lower_upper "../../luac/luac.c"
copy_lower_upper "../../lapi.[ch]"
copy_lower_upper "../../lauxlib.[ch]"
copy_lower_upper "../../lbaselib.c"
copy_lower_upper "../../lcode.[ch]"
copy_lower_upper "../../lcorolib.c"
copy_lower_upper "../../lctype.[ch]"
copy_lower_upper "../../ldblib.c"
copy_lower_upper "../../ldebug.[ch]"
copy_lower_upper "../../ldo.[ch]"
copy_lower_upper "../../ldump.c"
copy_lower_upper "../../lfunc.[ch]"
copy_lower_upper "../../lgc.[ch]"
copy_lower_upper "../../linit.c"
copy_lower_upper "../../liolib.c"
copy_lower_upper "../../ljumptab.h"
copy_lower_upper "../../llex.[ch]"
copy_lower_upper "../../llimits.h"
copy_lower_upper "../../lmathlib.c"
copy_lower_upper "../../lmem.[ch]"
copy_lower_upper "../../loadlib.c"
copy_lower_upper "../../lobject.[ch]"
copy_lower_upper "../../lopcodes.[ch]"
copy_lower_upper "../../lopnames.h"
copy_lower_upper "../../loslib.c"
copy_lower_upper "../../lparser.[ch]"
copy_lower_upper "../../lprefix.h"
copy_lower_upper "../../lstate.[ch]"
copy_lower_upper "../../lstring.[ch]"
copy_lower_upper "../../lstrlib.c"
copy_lower_upper "../../ltable.[ch]"
copy_lower_upper "../../ltablib.c"
copy_lower_upper "../../ltm.[ch]"
copy_lower_upper "../../lua.[ch]"
copy_lower_upper "../../luaconf.h"
copy_lower_upper "../../lualib.h"
copy_lower_upper "../../lundump.[ch]"
copy_lower_upper "../../lutf8lib.c"
copy_lower_upper "../../lvm.[ch]"
copy_lower_upper "../../lzio.[ch]"

# Zip up files, converting LF to CRLF
pushd 8.3
zip -l -r LUA.ZIP LUA
popd
