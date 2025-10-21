#!/bin/bash
# Link with system C library

OUTPUT=$1
OBJECT=$2
ARCH=$3
PLATFORM=$4

case $PLATFORM in
    "macos")
        if [ "$ARCH" = "arm64" ]; then
            clang -arch arm64 -o "$OUTPUT" "$OBJECT" -lSystem
        else
            clang -arch x86_64 -o "$OUTPUT" "$OBJECT" -lSystem
        fi
        ;;
    "linux")
        if [ "$ARCH" = "arm64" ]; then
            gcc -o "$OUTPUT" "$OBJECT" -lc -lm
        else
            gcc -o "$OUTPUT" "$OBJECT" -lc -lm
        fi
        ;;
    *)
        echo "Unknown platform: $PLATFORM"
        exit 1
        ;;
esac