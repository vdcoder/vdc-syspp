#!/bin/bash

# Unified build script for Metal Server and Client

set -e

BUILD_CLIENT=true
BUILD_SERVER=true
BUILD_TYPE="Debug"
TO_STATIC=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --client-only)
            BUILD_SERVER=false
            shift
            ;;
        --server-only)
            BUILD_CLIENT=false
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --to-static)
            TO_STATIC=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--client-only|--server-only] [--release] [--to-static]"
            exit 1
            ;;
    esac
done

echo "╔════════════════════════════════════════╗"
echo "║  🏗️  Metal Full-Stack Build System    ║"
echo "╚════════════════════════════════════════╝"
echo ""

# Build client
if [ "$BUILD_CLIENT" = true ]; then
    echo "📦 Building WebAssembly Client..."
    if [ "$TO_STATIC" = true ]; then
        ./build-client.sh --to-static
    else
        ./build-client.sh
    fi
    echo ""
fi

# Build server
if [ "$BUILD_SERVER" = true ]; then
    echo "🔧 Building C++ Server ($BUILD_TYPE)..."
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
    cmake --build .
    cd ..
    echo "✅ Server built successfully!"
    echo "   Binary: $(pwd)/build/MetalServer"
    echo ""
fi

# Setup instructions
echo "╔════════════════════════════════════════╗"
echo "║  ✨ Build Complete!                   ║"
echo "╚════════════════════════════════════════╝"
echo ""

if [ "$BUILD_CLIENT" = true ] && [ "$BUILD_SERVER" = true ]; then
    if [ "$TO_STATIC" = true ]; then
        echo "To run the full-stack application:"
        echo "  cd build && ./MetalServer"
        echo ""
        echo "Then open: http://localhost:8080"
    else
        echo "To run with client files:"
        echo "  STATIC_PATH=src/client/output ./build/MetalServer"
        echo ""
        echo "Or copy client to static dir:"
        echo "  mkdir -p static && cp -r src/client/output/* static/"
        echo "  cd build && ./MetalServer"
    fi
elif [ "$BUILD_CLIENT" = true ]; then
    echo "Client files: src/client/output/"
    if [ "$TO_STATIC" = true ]; then
        echo "Copied to: static/"
    fi
elif [ "$BUILD_SERVER" = true ]; then
    echo "To run server:"
    echo "  cd build && ./MetalServer"
fi

echo ""
