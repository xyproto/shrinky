#!/bin/sh
SHRINKY_MODULE_PATH=shrinky
if [ -z $1 ]; then
  echo 'Shrinky 1.0.3'
  echo
  echo 'Provide the path to a C++ source file as the first argument.'
  echo
  echo 'Example:'
  echo '    shrinky examples/intro.cpp'
  echo
  exit 1
fi
FILENAME="$1"
shift
SRCDIR="$(dirname $FILENAME)"
[ -f "$SRCDIR/shrinky.h" ] || touch "$SRCDIR/shrinky.h"
if [ -d "/usr/lib/arm-linux-gnueabihf/mali-egl" ]; then # Mali
  python -m shrinky -v "$FILENAME" --rpath "/usr/local/lib" -lc -ldl -lgcc -lm -lEGL -lGLESv2 -lSDL3 -m dlfcn "$@"
else
  python -m shrinky -v "$FILENAME" "$@"
fi
if [ $? -ne 0 ]; then
  echo "${0}: compilation failed"
  exit 1
fi
