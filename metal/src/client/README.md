# Metal Client - Emscripten WebAssembly Demo

A demonstration of C++ and JavaScript communication using Emscripten and WebAssembly.

## � Note on IntelliSense

**IntelliSense warnings in `client.cpp` are expected and can be ignored.** 

Emscripten uses its own toolchain (LLVM/Clang with custom libc++) that differs from standard Linux C++ development. While the code compiles perfectly with `emcc`, VS Code's IntelliSense may show errors because it's configured for the Metal Server's environment.

**The important thing:** If `./build.sh` completes successfully, your code is correct! ✅

### Why This Happens

- **Metal Server**: Uses g++ and GNU libstdc++
- **Emscripten Client**: Uses emcc (Clang) and LLVM libc++
- **VS Code**: Can only use one C++ configuration at a time for IntelliSense

### Quick Verification

```bash
cd metal/src/client
./build.sh          # Should complete with no errors
cd output
python3 -m http.server 8000
# Open http://localhost:8000 and test!
```

If the build succeeds and the browser demo works, you're golden! 🎯

---

## 🏗️ Project Structure

This client showcases:
- **C++ to JavaScript bindings** - Call C++ functions from JavaScript
- **Class bindings** - Use C++ classes as JavaScript objects
- **Callbacks** - C++ calling JavaScript functions
- **State management** - Persistent state in C++ accessed from JS
- **Type safety** - Automatic type conversion between C++ and JS

## 🏗️ Project Structure

```
metal/src/client/
├── client.cpp      # C++ source with Emscripten bindings
├── index.html      # HTML interface for testing
├── build.sh        # Build script
├── README.md       # This file
└── output/         # Generated files (created by build.sh)
    ├── client.js   # JavaScript glue code
    ├── client.wasm # WebAssembly binary
    └── index.html  # Copied HTML file
```

## 🔧 Prerequisites

### Install Emscripten

```bash
# Clone the Emscripten SDK
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# Download and install the latest SDK tools
./emsdk install latest

# Activate the latest SDK
./emsdk activate latest

# Add Emscripten to PATH (run this in each new terminal)
source ./emsdk_env.sh
```

To make Emscripten available permanently, add this to your `~/.bashrc`:
```bash
echo 'source /path/to/emsdk/emsdk_env.sh' >> ~/.bashrc
```

### Verify Installation

```bash
emcc --version
```

You should see output like:
```
emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 3.x.x
```

## 🚀 Building

```bash
cd metal/src/client
./build.sh
```

The build script will:
1. Check for Emscripten installation
2. Compile `client.cpp` to WebAssembly
3. Generate `client.js` (glue code) and `client.wasm`
4. Copy `index.html` to the output directory

## 🌐 Running

### Option 1: Python HTTP Server (Recommended)

```bash
cd output
python3 -m http.server 8000
```

Then open http://localhost:8000 in your browser.

### Option 2: Node.js HTTP Server

```bash
cd output
npx http-server -p 8000
```

### Option 3: VS Code Live Server

1. Install "Live Server" extension
2. Right-click on `output/index.html`
3. Select "Open with Live Server"

## 🎯 Features Demonstrated

### 1. Standalone Functions
Simple C++ functions exposed to JavaScript:
```cpp
std::string sayHello();
int add(int a, int b);
double multiply(double a, double b);
```

JavaScript usage:
```javascript
const greeting = Module.sayHello();
const sum = Module.add(5, 10);
const product = Module.multiply(3.14, 2.0);
```

### 2. C++ Classes
Complete C++ class with constructor, methods, and state:
```cpp
class MetalClient {
    std::string greet(const std::string& name);
    int calculate(int a, int b);
    long getTimestamp();
    // ... more methods
};
```

JavaScript usage:
```javascript
const client = new Module.MetalClient("http://localhost:8080");
const greeting = client.greet("World");
const result = client.calculate(5, 12);
```

### 3. State Management
The C++ class maintains internal state accessible from JavaScript:
```javascript
client.setServerUrl("http://new-url.com");
const url = client.getServerUrl();
const count = client.getMessageCount();
client.resetCount();
```

### 4. Callbacks
C++ can call JavaScript functions:
```cpp
void processWithCallback(int value, val callback) {
    int result = value * 2;
    callback(result);
}
```

JavaScript usage:
```javascript
Module.processWithCallback(42, (result) => {
    console.log("C++ returned:", result);
});
```

## 📖 Code Explanation

### Emscripten Bindings

The `EMSCRIPTEN_BINDINGS` macro creates the bridge between C++ and JavaScript:

```cpp
EMSCRIPTEN_BINDINGS(metal_client_module) {
    // Bind a class
    class_<MetalClient>("MetalClient")
        .constructor<std::string>()
        .function("greet", &MetalClient::greet)
        .function("calculate", &MetalClient::calculate);

    // Bind standalone functions
    function("sayHello", &sayHello);
    function("add", &add);
}
```

### Build Flags Explained

```bash
emcc client.cpp -o output/client.js -lembind ...
```

- `-lembind` - Enable Embind for C++/JS bindings
- `-std=c++17` - Use C++17 standard
- `-O3` - Optimize for performance
- `-s WASM=1` - Generate WebAssembly
- `-s ALLOW_MEMORY_GROWTH=1` - Allow dynamic memory allocation
- `-s MODULARIZE=1` - Create a module pattern
- `-s EXPORT_NAME="Module"` - Name the exported module
- `--bind` - Enable Embind bindings

## 🔍 Debugging

### Browser Console
Open browser DevTools (F12) to see:
- Module loading status
- Available functions: `Object.keys(Module)`
- C++ console output via `std::cout`

### Common Issues

**"Module is not defined"**
- Make sure `client.js` is loaded before your script
- Wait for `Module.onRuntimeInitialized` callback

**"Cannot find emcc"**
- Run `source /path/to/emsdk/emsdk_env.sh`
- Verify with `which emcc`

**CORS errors when loading**
- Must serve via HTTP server, not `file://`
- Use Python or Node HTTP server as shown above

## 🚀 Next Steps

### Connect to Metal Server

Extend the client to communicate with the Metal Server:

```cpp
// In client.cpp - add HTTP request capability
#include <emscripten/fetch.h>

void fetchFromServer(const std::string& endpoint) {
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    
    std::string url = serverUrl + endpoint;
    emscripten_fetch(&attr, url.c_str());
}
```

### Add More Features

- JSON parsing/serialization
- WebSocket connection to server
- Local storage persistence
- Canvas rendering
- Multi-threading with Web Workers

## 📚 Resources

- [Emscripten Documentation](https://emscripten.org/docs/)
- [Embind Documentation](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html)
- [WebAssembly.org](https://webassembly.org/)
- [MDN WebAssembly Guide](https://developer.mozilla.org/en-US/docs/WebAssembly)

## 📄 License

Part of the Metal Server project.
