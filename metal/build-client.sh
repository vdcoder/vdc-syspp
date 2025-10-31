#!/bin/bash

# Build script for Metal WebAssembly Client

set -e

echo "🔨 Building Metal WebAssembly Client..."

cd "$(dirname "$0")/src/client"

# Check if Emscripten is available
if ! command -v emcc &> /dev/null; then
    echo "❌ Error: Emscripten not found!"
    echo "   Please source the Emscripten environment:"
    echo "   source ~/emsdk/emsdk_env.sh"
    exit 1
fi

# Run the client build script
./build.sh

# Copy output to static directory if requested
if [ "$1" == "--to-static" ]; then
    STATIC_DIR="../../static"
    mkdir -p "$STATIC_DIR"
    cp -r output/* "$STATIC_DIR/"
    echo "✅ Client built and copied to $STATIC_DIR"
else
    echo "✅ Client built successfully!"
    echo "   Output: $(pwd)/output/"
    echo "   To use with server, run: ./build-client.sh --to-static"
fi
