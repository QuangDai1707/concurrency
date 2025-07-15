#!/bin/bash

# Check if file path argument is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <cpp_source_file>"
    exit 1
fi

SOURCE_FILE="$1"

# Check if the file exists
if [ ! -f "$SOURCE_FILE" ]; then
    echo "Error: File '$SOURCE_FILE' not found."
    exit 1
fi

# Get the filename without extension for output
BASENAME=$(basename "$SOURCE_FILE" .cpp)
OUTPUT_FILE="${BASENAME}.out"

# Compile the C++ source file
COMPILE_START=$(date +%s.%N)
g++ "$SOURCE_FILE" -o "$OUTPUT_FILE"
COMPILE_END=$(date +%s.%N)

# Check if compilation succeeded
if [ $? -eq 0 ]; then
    RUN_START=$(date +%s.%N)
    ./"$OUTPUT_FILE"
    RUN_END=$(date +%s.%N)
    echo ""
    COMPILE_TIME=$(echo "$COMPILE_END - $COMPILE_START" | bc)
    echo "Compile time: ${COMPILE_TIME}s"
    RUN_TIME=$(echo "$RUN_END - $RUN_START" | bc)
    echo "Run time: ${RUN_TIME}s"
    rm "$OUTPUT_FILE"
else
    echo "Build failed."
    exit 1
fi
