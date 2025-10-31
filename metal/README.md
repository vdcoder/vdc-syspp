# Metal Server# Metal Server



A full-stack C++ application featuring a fast HTTP server built with oatpp and a WebAssembly client compiled with Emscripten.A fast and lean C++ HTTP server built with oatpp framework.



## Features## Features



- ⚡ Fast and lightweight using oatpp framework- ⚡ Fast and lightweight using oatpp

- 🌐 WebAssembly client with C++/JavaScript interop- 🐳 Docker support for easy deployment

- 📦 Unified build system for client and server- 🚂 Railway-ready with automatic port configuration

- 🐳 Docker support for easy deployment- 🔧 CMake build system

- 🚂 Railway-ready with automatic port configuration- 🌐 RESTful API endpoints

- 🔧 CMake build system for C++ server

- 🎯 Static file serving for client app## Prerequisites

- 🔒 Security features (directory traversal protection)

### For WSL2/Linux Development

## Quick Start```bash

# Install build tools

### Prerequisitessudo apt-get update

sudo apt-get install -y build-essential cmake git

**For WSL2/Linux Development:**

```bash# Install oatpp

# Install build toolscd /tmp

sudo apt-get updategit clone --depth=1 --branch 1.3.0 https://github.com/oatpp/oatpp.git

sudo apt-get install -y build-essential cmake git python3cd oatpp

mkdir build && cd build

# Install oatppcmake ..

cd /tmpsudo make install

git clone --depth=1 --branch 1.3.0 https://github.com/oatpp/oatpp.git```

cd oatpp && mkdir build && cd build

cmake .. && sudo make install## Building



# Install Emscripten (for WebAssembly client)### Using CMake (WSL2/Linux)

cd ~```bash

git clone https://github.com/emscripten-core/emsdk.gitcd metal

cd emsdkmkdir build && cd build

./emsdk install latestcmake ..

./emsdk activate latestcmake --build .

echo 'source ~/emsdk/emsdk_env.sh' >> ~/.bashrc```

source ~/emsdk/emsdk_env.sh

```### Using Docker

```bash

### Build and Runcd metal

docker build -t metal-server .

```bashdocker run -p 8080:8080 metal-server

# Build everything (client + server)```

./build-all.sh --to-static

## Running

# Run the server

cd build && ./MetalServer### Local Development

```bash

# Open http://localhost:8080 in your browser# From build directory

```./MetalServer



That's it! You now have a full-stack C++ application running.# Or with custom port

PORT=3000 ./MetalServer

## Building```



### Quick Build Options### Docker

```bash

```bashdocker run -p 8080:8080 metal-server

# Build both client and server, copy to static directory```

./build-all.sh --to-static

## API Endpoints

# Build only the client

./build-all.sh --client-only- `GET /` - Welcome message

- `GET /health` - Health check endpoint

# Build only the server- `GET /api/hello?name=<name>` - Greeting endpoint

./build-all.sh --server-only

## Deploying to Railway

# Build in Release mode (optimized)

./build-all.sh --release --to-static1. Install Railway CLI or use the web interface

```2. Create a new project on Railway

3. Connect your GitHub repository

### Manual Build4. Railway will automatically detect the Dockerfile and deploy

5. The server will bind to Railway's provided PORT environment variable

**Client (WebAssembly):**

```bash### Using Railway CLI

cd src/client```bash

./build.sh# Install Railway CLI

# Output: output/client.js, output/client.wasm, output/index.htmlnpm install -g @railway/cli

```

# Login

**Server (C++):**railway login

```bash

mkdir build && cd build# Initialize and deploy

cmake -DCMAKE_BUILD_TYPE=Debug ..railway init

cmake --build .railway up

``````



### Docker Build## Development in VS Code with WSL2

```bash

docker build -t metal-server .1. Install "Remote - WSL" extension in VS Code

docker run -p 8080:8080 metal-server2. Open WSL2 terminal in VS Code (Ctrl+`)

```3. Navigate to the project: `cd /mnt/c/Users/vdcod/OneDrive/Desktop/OneDrive/vdcoder.com/vdc-syspp/metal`

4. Open in VS Code: `code .`

The Docker build automatically:5. Build and run as described above

- Installs Emscripten

- Builds the WebAssembly client## Project Structure

- Builds the C++ server

- Packages everything into a minimal production image```

metal/

## Running├── src/

│   ├── main.cpp              # Application entry point

### Local Development│   ├── AppComponent.hpp      # Dependency injection configuration

│   └── controller/

**Full-Stack Mode** (with client app):│       └── ApiController.hpp # API endpoint definitions

```bash├── CMakeLists.txt            # Build configuration

cd build├── Dockerfile                # Container configuration

./MetalServer└── README.md                 # This file

# Access at http://localhost:8080```

```

## Next Steps

**API-Only Mode** (server only):

```bash- Add more endpoints in `ApiController.hpp`

cd build- Implement business logic

./MetalServer- Add database integration

# Static files not found - runs in API-only mode- Set up authentication/authorization

```- Configure CORS for web clients


**Custom Static Path**:
```bash
STATIC_PATH=../src/client/output ./build/MetalServer
```

**Custom Port**:
```bash
PORT=3000 ./build/MetalServer
```

### Using Docker
```bash
docker run -p 8080:8080 metal-server
```

## Application Modes

Metal Server automatically detects available static files and runs in the appropriate mode:

### Full-Stack Mode
When static files are found:
- Serves WebAssembly client at `/`
- Serves API endpoints at `/api/*` and `/health`
- Serves static files (JS, WASM, CSS, etc.)

```
┌─────────────────────────────────────┐
│  🚀 Metal Server is Running!       │
├─────────────────────────────────────┤
│  Port: 8080                         │
│  Mode: Full-Stack                   │
│  Endpoints:                         │
│    GET  /           (Client App)    │
│    GET  /{file}     (Static Files)  │
│    GET  /health                     │
│    GET  /api/hello?name=<name>      │
└─────────────────────────────────────┘
```

### API-Only Mode
When static files are not found:
- Only serves API endpoints
- Displays instructions for building the client

```
┌─────────────────────────────────────┐
│  🚀 Metal Server is Running!       │
├─────────────────────────────────────┤
│  Port: 8080                         │
│  Mode: API Only                     │
│  Endpoints:                         │
│    GET  /health                     │
│    GET  /api/hello?name=<name>      │
└─────────────────────────────────────┘
```

## API Endpoints

- `GET /` - WebAssembly client (Full-Stack mode)
- `GET /health` - Health check endpoint
- `GET /api/hello?name=<name>` - Greeting endpoint
- `GET /{filename}` - Static files (HTML, JS, WASM, CSS, etc.)

## WebAssembly Client Features

The client demonstrates:
- **C++ Classes in WASM**: `MetalClient` class with methods exposed to JavaScript
- **Standalone Functions**: `sayHello()`, `add()`, `multiply()`
- **Callbacks**: C++ can call JavaScript functions
- **State Management**: C++ object state persists across calls
- **Type Safety**: Embind provides type-safe bindings

Example usage from browser:
```javascript
// Create instance
const client = new Module.MetalClient("Alice");

// Call methods
console.log(client.greet());           // "Hello, Alice!"
console.log(client.calculate(5, 3));   // 8
console.log(client.getTimestamp());    // Current timestamp

// Standalone functions
console.log(Module.sayHello("Bob"));   // "Hello from C++, Bob!"
console.log(Module.add(10, 20));       // 30
```

## Deploying to Railway

Railway deployment is fully automated:

```bash
# Install Railway CLI (optional)
npm install -g @railway/cli

# Deploy via CLI
railway login
railway init
railway up

# Or connect via web interface:
# 1. Go to railway.app
# 2. Create new project from GitHub repo
# 3. Railway auto-detects Dockerfile and deploys
```

Railway will:
- Build both client and server
- Set the PORT environment variable
- Provide a public HTTPS URL
- Handle automatic deploys on git push

## Project Structure

```
metal/
├── src/
│   ├── main.cpp                   # Server entry point
│   ├── AppComponent.hpp           # oatpp components
│   ├── controller/
│   │   ├── ApiController.hpp      # REST API endpoints
│   │   └── StaticController.hpp   # Static file serving
│   └── client/
│       ├── client.cpp             # WebAssembly client (C++)
│       ├── index.html             # Client UI
│       ├── build.sh               # Client build script
│       ├── output/                # Build output
│       └── README.md              # Client documentation
├── static/                        # Production static files
├── build/                         # CMake build directory
├── build-all.sh                   # Unified build script
├── build-client.sh                # Client build wrapper
├── CMakeLists.txt                 # Server build configuration
├── Dockerfile                     # Docker configuration
├── README.md                      # This file
├── INTEGRATION.md                 # Integration guide
└── DEBUGGING.md                   # Debugging guide
```

## Development Workflow

### VS Code with WSL2

1. Install "Remote - WSL" extension
2. Open project in WSL: Press F1 → "WSL: Open Folder in WSL"
3. Available VS Code tasks (Ctrl+Shift+B):
   - Build Metal Server (Debug)
   - Build Metal Server (Release)
   - Clean Build
   - Run Metal Server

### Debugging

See [DEBUGGING.md](DEBUGGING.md) for detailed debugging instructions.

Quick start:
```bash
# Build with debug symbols
./build-all.sh --to-static

# In VS Code: Press F5 to start debugging
# Or manually: gdb ./build/MetalServer
```

### Integration Details

See [INTEGRATION.md](INTEGRATION.md) for comprehensive integration documentation, including:
- Architecture overview
- Build system details
- Static file serving
- Environment variables
- Troubleshooting
- Production considerations

## Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `PORT` | `8080` | Server port |
| `STATIC_PATH` | `./static` | Path to static files directory |

## Troubleshooting

### Static files not found
```bash
# Build client and copy to static directory
./build-client.sh --to-static

# Or create symlink
cd build && ln -s ../static static
```

### Emscripten not found
```bash
source ~/emsdk/emsdk_env.sh
```

### Server won't start
```bash
# Check if port is in use
lsof -i :8080

# Use different port
PORT=3001 ./build/MetalServer
```

See [INTEGRATION.md](INTEGRATION.md) for more troubleshooting tips.

## Next Steps

- **Add Features**: Extend `ApiController.hpp` with new endpoints
- **Enhance Client**: Add more functionality to `client.cpp`
- **Add Database**: Integrate PostgreSQL or SQLite
- **Authentication**: Implement JWT or session-based auth
- **WebSockets**: Add real-time communication
- **Testing**: Add unit tests and integration tests
- **CI/CD**: Set up automated testing and deployment
- **Monitoring**: Add logging, metrics, and observability

## Documentation

- [README.md](README.md) - This file (overview and quick start)
- [INTEGRATION.md](INTEGRATION.md) - Full-stack integration guide
- [DEBUGGING.md](DEBUGGING.md) - Debugging with GDB in VS Code
- [src/client/README.md](src/client/README.md) - WebAssembly client documentation

## License

[Your License Here]

## Contributing

Contributions welcome! Please read CONTRIBUTING.md first (if available).
