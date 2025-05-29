#!/bin/bash

set -e

BIN_DIR="$HOME/l2_project/bin"
CXX_PROGRAM="$BIN_DIR/parser"
PY_SCRIPT="$BIN_DIR/plot.py"

if [[ $# -eq 0 ]]; then
  echo "Usage: $0 file1 file2 ..."
  exit 1
fi

OUTPUT=$("$CXX_PROGRAM" "$@")
echo "$OUTPUT" | "$PY_SCRIPT"
