// Emscripten WebAssembly Client
// Note: IntelliSense may show errors here due to Emscripten's custom toolchain.
// These can be safely ignored - the code compiles correctly with emcc.
// Build with: ./build.sh

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <iostream>
#include <ctime>

using namespace emscripten;

// Simple C++ class that will be exposed to JavaScript
class MetalClient {
private:
    std::string serverUrl;
    int messageCount;

public:
    MetalClient(const std::string& url) 
        : serverUrl(url), messageCount(0) {
        std::cout << "MetalClient initialized with URL: " << url << std::endl;
    }

    // Get a greeting from C++
    std::string greet(const std::string& name) {
        messageCount++;
        return "Hello from C++, " + name + "! (Message #" + std::to_string(messageCount) + ")";
    }

    // Calculate something in C++
    int calculate(int a, int b) {
        return a * a + b * b;
    }

    // Get current timestamp
    long getTimestamp() {
        return static_cast<long>(std::time(nullptr));
    }

    // Get server URL
    std::string getServerUrl() const {
        return serverUrl;
    }

    // Set server URL
    void setServerUrl(const std::string& url) {
        serverUrl = url;
        std::cout << "Server URL updated to: " << url << std::endl;
    }

    // Get message count
    int getMessageCount() const {
        return messageCount;
    }

    // Reset message count
    void resetCount() {
        messageCount = 0;
        std::cout << "Message count reset" << std::endl;
    }
};

// Standalone C++ functions
std::string sayHello() {
    return "Hello from C++ standalone function!";
}

int add(int a, int b) {
    return a + b;
}

double multiply(double a, double b) {
    return a * b;
}

// Function that takes a JavaScript callback
void processWithCallback(int value, val callback) {
    int result = value * 2;
    callback(result);
}

// Bind C++ code to JavaScript using Emscripten
EMSCRIPTEN_BINDINGS(metal_client_module) {
    // Bind the MetalClient class
    class_<MetalClient>("MetalClient")
        .constructor<std::string>()
        .function("greet", &MetalClient::greet)
        .function("calculate", &MetalClient::calculate)
        .function("getTimestamp", &MetalClient::getTimestamp)
        .function("getServerUrl", &MetalClient::getServerUrl)
        .function("setServerUrl", &MetalClient::setServerUrl)
        .function("getMessageCount", &MetalClient::getMessageCount)
        .function("resetCount", &MetalClient::resetCount);

    // Bind standalone functions
    function("sayHello", &sayHello);
    function("add", &add);
    function("multiply", &multiply);
    function("processWithCallback", &processWithCallback);
}
