#!/bin/bash

# Build script for C++ Framework

set -e

echo "🔨 Building C++ Framework (Naked Version)..."

# Check if emcc is available
if ! command -v emcc &> /dev/null; then
    echo "❌ Error: Emscripten not found!"
    echo "   Please source the Emscripten environment:"
    echo "   source ~/emsdk/emsdk_env.sh"
    exit 1
fi

# Create output directory
mkdir -p output

# Compile with Emscripten
echo "📦 Compiling framework.cpp to WebAssembly..."

emcc framework.cpp \
    -o output/framework.js \
    -lembind \
    -std=c++17 \
    -O3 \
    -s WASM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="Module" \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    --bind

# Copy HTML file
echo "📄 Copying index.html to output..."
cp index.html output/

echo ""
echo "✅ Build completed successfully!"
echo ""
echo "📁 Output files:"
echo "   - output/framework.js (JavaScript glue code)"
echo "   - output/framework.wasm (WebAssembly binary)"
echo "   - output/index.html (Demo page)"
echo ""
echo "🚀 To run:"
echo "   cd output"
echo "   python3 -m http.server 8000"
echo "   # Then open http://localhost:8000"
echo ""
