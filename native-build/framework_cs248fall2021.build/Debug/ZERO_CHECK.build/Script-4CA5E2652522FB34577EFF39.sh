#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/nouraalhajry/Desktop/projects/framework_cs248fall2021-main/native-build
  make -f /Users/nouraalhajry/Desktop/projects/framework_cs248fall2021-main/native-build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/nouraalhajry/Desktop/projects/framework_cs248fall2021-main/native-build
  make -f /Users/nouraalhajry/Desktop/projects/framework_cs248fall2021-main/native-build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/nouraalhajry/Desktop/projects/framework_cs248fall2021-main/native-build
  make -f /Users/nouraalhajry/Desktop/projects/framework_cs248fall2021-main/native-build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/nouraalhajry/Desktop/projects/framework_cs248fall2021-main/native-build
  make -f /Users/nouraalhajry/Desktop/projects/framework_cs248fall2021-main/native-build/CMakeScripts/ReRunCMake.make
fi

