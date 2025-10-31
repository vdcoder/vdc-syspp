# Multi-stage build for minimal final image
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    xz-utils \
    && rm -rf /var/lib/apt/lists/*

# Install Emscripten
WORKDIR /emsdk
RUN git clone --depth=1 https://github.com/emscripten-core/emsdk.git . && \
    ./emsdk install latest && \
    ./emsdk activate latest

# Build oatpp
WORKDIR /tmp
RUN git clone --depth=1 --branch 1.3.0 https://github.com/oatpp/oatpp.git && \
    cd oatpp && \
    mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install

# Copy source code
WORKDIR /app
COPY . .

# Build the WebAssembly client
WORKDIR /app/src/client
RUN chmod +x build.sh && /bin/bash -c "source /emsdk/emsdk_env.sh && ./build.sh"

# Copy client output to static directory
WORKDIR /app
RUN mkdir -p static && cp -r src/client/output/* static/

# Build the C++ server
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . -j$(nproc)

# Production stage - minimal image
FROM ubuntu:22.04

# Install only runtime dependencies
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Copy the built binary
COPY --from=builder /app/build/MetalServer /usr/local/bin/MetalServer

# Copy static files (client app)
COPY --from=builder /app/static /app/static

# Copy oatpp shared libraries
COPY --from=builder /usr/local/lib/liboatpp-*.so* /usr/local/lib/

# Update library cache
RUN ldconfig

# Set working directory
WORKDIR /app

# Expose port (Railway will set PORT env var)
EXPOSE 8080

# Run the server with static files
CMD ["MetalServer"]
