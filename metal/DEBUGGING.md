# Debugging Metal Server in VS Code

## 🐛 Setup Complete!

Your debugging environment is now configured with:
- **GDB** debugger in WSL2
- **VS Code** debug configurations
- **Debug symbols** enabled in builds
- **Build tasks** for easy compilation

## 🚀 Quick Start

### Method 1: Press F5 (Recommended)
1. Open any `.cpp` or `.hpp` file in the `metal` folder
2. Set breakpoints by clicking left of line numbers (red dots)
3. Press **F5** - it will build and launch with debugger attached
4. Test endpoints while debugging:
   - http://localhost:8080/
   - http://localhost:8080/health
   - http://localhost:8080/api/hello?name=Test

### Method 2: Debug Panel
1. Click the **Run and Debug** icon in the sidebar (Ctrl+Shift+D)
2. Select **(gdb) Launch Metal Server** from dropdown
3. Click green play button

### Method 3: Attach to Running Process
1. Start server manually in terminal
2. In Debug panel, select **(gdb) Attach to Metal Server**
3. Click play - VS Code will show process list
4. Select `MetalServer` process

## 🔧 Build Tasks

Access via **Terminal → Run Task** or **Ctrl+Shift+B**:

- **Build Metal Server** (default) - Debug build with symbols
- **Build Metal Server (Release)** - Optimized production build
- **Clean Build** - Remove build directory
- **Run Metal Server** - Run without debugging

## 🎯 Debugging Features

### Set Breakpoints
- Click left margin in any `.cpp` or `.hpp` file
- Conditional breakpoints: Right-click breakpoint → Edit Breakpoint

### While Debugging
- **F10** - Step Over
- **F11** - Step Into
- **Shift+F11** - Step Out
- **F5** - Continue
- **Shift+F5** - Stop

### Inspect Variables
- Hover over variables to see values
- **Variables** panel shows local/global variables
- **Watch** panel for custom expressions
- **Call Stack** shows function call hierarchy

## 📋 Example Debug Session

1. Set breakpoint in `ApiController.hpp` at line in the `hello` endpoint
2. Press **F5** to start debugging
3. Open browser: http://localhost:8080/api/hello?name=Debug
4. VS Code pauses at breakpoint
5. Inspect `name` variable value
6. Step through code with F10
7. Press F5 to continue

## 🔍 Troubleshooting

### "Unable to start debugging"
- Make sure no server is already running on port 8080
- Check that GDB is installed: `wsl gdb --version`

### Breakpoints not hitting
- Ensure Debug build (not Release): Check tasks.json uses `-DCMAKE_BUILD_TYPE=Debug`
- Rebuild with debug symbols: Run "Build Metal Server" task

### Source file not found
- Check `sourceFileMap` in `.vscode/launch.json`
- Verify file paths match your system

## 🎓 Pro Tips

1. **Debug builds are slower** - Use Release builds for performance testing
2. **Pretty printing enabled** - STL containers display nicely in debugger
3. **Multiple instances** - Change PORT env var to run multiple servers
4. **Remote debugging** - GDB over WSL works from Windows seamlessly

## 📚 Recommended Extensions

VS Code will prompt to install:
- **C/C++** - IntelliSense and debugging
- **CMake Tools** - CMake integration
- **Remote - WSL** - WSL development support

Happy Debugging! 🎉
