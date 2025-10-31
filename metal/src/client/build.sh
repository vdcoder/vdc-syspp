#!/bin/bash

# Build script for Emscripten client
# This script compiles C++ code to WebAssembly using Emscripten

set -e  # Exit on error

echo "🔨 Building Metal Client with Emscripten..."

# Check if emcc is available
if ! command -v emcc &> /dev/null; then
    echo "❌ Error: Emscripten (emcc) not found!"
    echo "Please install Emscripten from: https://emscripten.org/docs/getting_started/downloads.html"
    echo ""
    echo "Quick install:"
    echo "  git clone https://github.com/emscripten-core/emsdk.git"
    echo "  cd emsdk"
    echo "  ./emsdk install latest"
    echo "  ./emsdk activate latest"
    echo "  source ./emsdk_env.sh"
    exit 1
fi

# Create output directory if it doesn't exist
mkdir -p output

# Compile C++ to WebAssembly
echo "📦 Compiling client.cpp to WebAssembly..."

emcc client.cpp \
    -o output/client.js \
    -lembind \
    -std=c++17 \
    -O3 \
    -s WASM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="Module" \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    --bind

# Copy HTML file to output
echo "📄 Copying index.html to output..."
cp index.html output/

echo ""
echo "✅ Build completed successfully!"
echo ""
echo "📁 Output files:"
echo "   - output/client.js (JavaScript glue code)"
echo "   - output/client.wasm (WebAssembly binary)"
echo "   - output/index.html (HTML interface)"
echo ""
echo "🚀 To run the client:"
echo "   cd output"
echo "   python3 -m http.server 8000"
echo "   # Then open http://localhost:8000 in your browser"
echo ""
