#!/bin/bash

ARGS="$@"

if [ -n "${SVF_DIR}" ] && [ -d "${SVF_DIR}/llvm-16.0.0.obj" ]; then
  export LLVM_DIR="${SVF_DIR}/llvm-16.0.0.obj"
  echo "LLVM_DIR: $LLVM_DIR"
elif [ -n "${LLVM_DIR}" ] && [ -d "${LLVM_DIR}" ]; then
  echo "LLVM_DIR: $LLVM_DIR"
else
  echo "LLVM_DIR not found!"
  exit 1
fi

rm -rf build/
mkdir build && cd build/

if [ -n "$ARGS" ]; then
  cmake -DCMAKE_BUILD_TYPE=Debug .. -DSUBDIRS="$ARGS"
else
  cmake -DCMAKE_BUILD_TYPE=Debug ..
fi

make -j4
