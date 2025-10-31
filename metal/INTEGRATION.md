# Metal Full-Stack Integration Guide

This document explains how the Metal Server and WebAssembly client are integrated into a single full-stack application.

## Architecture

The Metal application consists of two main components:

1. **C++ Backend Server** (using oatpp framework)
   - REST API endpoints at `/api/*`
   - Static file serving for the client app
   - Health check endpoint at `/health`

2. **WebAssembly Client** (C++ compiled to WASM using Emscripten)
   - Interactive browser-based UI
   - C++/JavaScript bidirectional communication
   - Demonstrates WASM capabilities with MetalClient class

## Build System

### Build Scripts

#### `build-all.sh` - Unified Build Script
Build both client and server with one command:

```bash
# Build everything (Debug mode)
./build-all.sh

# Build everything and copy client to static directory
./build-all.sh --to-static

# Build only the client
./build-all.sh --client-only

# Build only the server
./build-all.sh --server-only

# Build in Release mode
./build-all.sh --release
```

#### `build-client.sh` - Client Only
Build just the WebAssembly client:

```bash
# Build client (output to src/client/output/)
./build-client.sh

# Build and copy to static directory
./build-client.sh --to-static
```

### CMake Build
The traditional CMake build still works for server-only builds:

```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

## Running the Application

### Local Development

After building with `--to-static`:

```bash
cd build
./MetalServer
```

Or use environment variable to specify static files path:

```bash
STATIC_PATH=../static ./build/MetalServer
# Or
STATIC_PATH=src/client/output ./build/MetalServer
```

The server will automatically detect if static files are available and run in the appropriate mode:

- **Full-Stack Mode**: Serves both API and client app
- **API Only Mode**: Only serves API endpoints (when static files not found)

### Access Points

When running in Full-Stack mode:

- **Client App**: http://localhost:8080
- **API Health**: http://localhost:8080/health
- **API Hello**: http://localhost:8080/api/hello?name=YourName

## Docker Deployment

### Build Docker Image

```bash
docker build -t metal-server .
```

The Dockerfile:
1. Installs Emscripten in the builder stage
2. Builds the WebAssembly client
3. Builds the C++ server
4. Copies both to a minimal production image

### Run Docker Container

```bash
docker run -p 8080:8080 metal-server
```

## Railway Deployment

The application is Railway-ready:

1. **Dockerfile** handles complete build process
2. **PORT environment variable** is automatically configured
3. **STATIC_PATH** defaults to `./static` (automatically set up in Docker)

### Deploy to Railway

```bash
# Install Railway CLI
npm install -g @railway/cli

# Login and deploy
railway login
railway init
railway up
```

Railway will:
- Detect the Dockerfile
- Build both client and server
- Deploy with automatic PORT configuration
- Provide a public URL

## Static File Serving

The `StaticController` class handles static file serving:

- **Root endpoint** (`/`): Serves `index.html`
- **File endpoint** (`/{filename}`): Serves requested files
- **MIME type detection**: Automatically sets content types for .html, .js, .wasm, etc.
- **Security**: Prevents directory traversal attacks (blocks `../` in paths)
- **CORS headers**: Configured for WebAssembly support

## Development Workflow

### Quick Start

```bash
# First time setup
./build-all.sh --to-static

# Run the server
cd build && ./MetalServer

# Open http://localhost:8080 in your browser
```

### Iterative Development

**Client changes only:**
```bash
cd src/client
./build.sh
cp -r output/* ../../static/
# Server will serve updated files (may need browser refresh)
```

**Server changes only:**
```bash
./build-all.sh --server-only
cd build && ./MetalServer
```

**Full rebuild:**
```bash
./build-all.sh --to-static
```

## Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `PORT` | `8080` | Server port |
| `STATIC_PATH` | `./static` | Path to static files directory |

## File Structure

```
metal/
├── src/
│   ├── main.cpp                 # Server entry point
│   ├── AppComponent.hpp         # oatpp component configuration
│   ├── controller/
│   │   ├── ApiController.hpp    # REST API endpoints
│   │   └── StaticController.hpp # Static file serving
│   └── client/
│       ├── client.cpp           # WebAssembly client code
│       ├── index.html           # Client UI
│       ├── build.sh             # Client build script
│       └── output/              # Build output (client.js, client.wasm)
├── static/                      # Production static files
├── build/                       # CMake build directory
├── build-all.sh                 # Unified build script
├── build-client.sh              # Client build wrapper
├── Dockerfile                   # Docker configuration
└── CMakeLists.txt               # CMake configuration
```

## Troubleshooting

### Static files not found

```
⚠️  Static files not found at: ./static
```

**Solution**: Build the client and copy to static directory:
```bash
./build-client.sh --to-static
```

Or create symlink in build directory:
```bash
cd build && ln -s ../static static
```

### WebAssembly module doesn't load

Check browser console for CORS errors. The StaticController sets required headers:
- `Cross-Origin-Opener-Policy: same-origin`
- `Cross-Origin-Embedder-Policy: require-corp`

### Emscripten not found during build

**In Docker**: This is handled automatically by the Dockerfile

**Local development**: Source the Emscripten environment:
```bash
source ~/emsdk/emsdk_env.sh
```

Add to your `~/.bashrc` for persistence:
```bash
echo 'source ~/emsdk/emsdk_env.sh' >> ~/.bashrc
```

## Production Considerations

### Performance
- Client files are read from disk on each request (consider caching for production)
- Use Release build mode: `./build-all.sh --release --to-static`
- Consider using a CDN for static files in high-traffic scenarios

### Security
- Directory traversal protection is implemented
- Consider adding authentication for sensitive endpoints
- Use HTTPS in production (Railway provides this automatically)

### Monitoring
- Check `/health` endpoint for server status
- Monitor Docker logs: `docker logs <container-id>`
- Railway provides built-in logging and metrics

## Next Steps

- Add persistent state management
- Implement WebSocket support for real-time communication
- Add more client features and API endpoints
- Set up CI/CD pipeline with automated testing
- Add monitoring and observability
