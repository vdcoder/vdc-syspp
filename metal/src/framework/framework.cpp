#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/html5.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include "JString.hpp"

using namespace emscripten;

// Forward declaration
class Renderer;

// Global renderer instance (simple for now)
Renderer* g_renderer = nullptr;

// ============================================================================
// Event System - Generic callback registry
// ============================================================================
using EventCallback = std::function<void()>;
std::unordered_map<int, EventCallback> g_eventHandlers;
int g_nextEventId = 0;

// Register a callback and return a unique ID
int registerCallback(EventCallback callback) {
  int id = g_nextEventId++;
  g_eventHandlers[id] = callback;
  return id;
}

// Call a registered callback from JavaScript
void invokeCallback(int id) {
  auto it = g_eventHandlers.find(id);
  if (it != g_eventHandlers.end()) {
    it->second();
  }
}

// ============================================================================
// Base Classes
// ============================================================================

class IInvalidator {
public:
  virtual void invalidate() = 0;
};

class ComponentBase: public IInvalidator {
private:
    IInvalidator* invalidator;
public:
    ComponentBase(IInvalidator* invalidator) : invalidator(invalidator) {}
    
    virtual std::string render() = 0;
    
    void invalidate() {
        invalidator->invalidate();
    }
};

class AppBase: public ComponentBase {
public:
  AppBase(IInvalidator* invalidator) : ComponentBase(invalidator) {}
  virtual void start() = 0;
};



// ============================================================================
// Renderer - Handles scheduling and DOM updates
// ============================================================================
class Renderer: public IInvalidator {
private:
  AppBase* app = nullptr;
  std::string oldHTML = "";
  bool hasPatches = false;
  bool frameRequested = false;

  // requestAnimationFrame callback
  static bool onFrame(double timestamp, void* userData) {
    auto* renderer = static_cast<Renderer*>(userData);
    renderer->frameRequested = false;

    if (renderer->hasPatches) {
      renderer->applyPatches();
      renderer->hasPatches = false;
    }
    
    return false; // We don't loop on frames automatically
  }

  void applyPatches() {
    // Generate new HTML
    std::string newHTML = app->render();

    // Simple diff: compare strings
    if (newHTML != oldHTML) {
      // Apply to DOM
      val document = val::global("document");
      val root = document.call<val>("getElementById", val("app-root"));
      
      if (!root.isNull() && !root.isUndefined()) {
        root.set("innerHTML", newHTML);
        
        // Reattach event listeners after innerHTML update
        setupEventListeners();
      }

      oldHTML = newHTML;
    }
  }

  void setupEventListeners() {
    // Note: With the new system, we don't need to manually attach listeners
    // The onclick attributes in HTML contain data-event-id that we set up
    // We just need to ensure the global handler is set up
    val window = val::global("window");
    if (window["__eventHandlerSetup"].isUndefined()) {
      // Set up one-time global event handler
      window.set("__eventHandlerSetup", true);
      window.set("invokeCallback", val::module_property("invokeCallback"));
    }
  }

public:
  void setApp(AppBase* a_app) {
    this->app = a_app;
  }

  virtual void invalidate() {
    hasPatches = true;

    if (!frameRequested) {
      emscripten_request_animation_frame(onFrame, this);
      frameRequested = true;
    }
  }

  void start() {
    invalidate();
  }
};

class MyComponent : public ComponentBase {
public:
  MyComponent(IInvalidator* invalidator) : ComponentBase(invalidator) {}

  virtual std::string render() {
    return 
      "<div style='font-family: sans-serif; padding: 20px;'>"
        "<h1>MyComponent</h1>"
      "</div>";
  }
};

class App : public AppBase {
private:
  // Simple state
  int counter = 0;
  JString message{"Hello from C++ with JString!"};  // Using JString!
  
  // Event handler IDs (integers now!)
  int incrementEventId;
  int resetEventId;
  int updateMessageEventId;

public:
  App(IInvalidator* invalidator) : AppBase(invalidator) {
    // Register event handlers with lambdas
    incrementEventId = registerCallback([this]() {
      counter++;
      invalidate();
    });
    
    resetEventId = registerCallback([this]() {
      counter = 0;
      invalidate();
    });
    
    updateMessageEventId = registerCallback([this]() {
      val document = val::global("document");
      val input = document.call<val>("getElementById", val("message-input"));
      if (!input.isNull()) {
        message = JString(input["value"].as<std::string>());
        invalidate();
      }
    });
  }

  virtual void start() {}

  // Render returns HTML string
  virtual std::string render() {
    return 
      "<div style='font-family: sans-serif; padding: 20px;'>"
        "<h1>" + message.toStdString() + "</h1>"
        "<p>Counter: " + std::to_string(counter) + "</p>"
        "<button onclick='invokeCallback(" + std::to_string(incrementEventId) + ")'>Increment</button>"
        "<button onclick='invokeCallback(" + std::to_string(resetEventId) + ")'>Reset</button>"
        "<input id='message-input' type='text' placeholder='Enter message' />"
        "<button onclick='invokeCallback(" + std::to_string(updateMessageEventId) + ")'>Update Message</button>" 
        + MyComponent(this).render() +
      "</div>";
  }
};

// ============================================================================
// Global app instance
// ============================================================================
App* g_app = nullptr;


// ============================================================================
// Main entry point
// ============================================================================
void startApp() {
  // Create renderer
  g_renderer = new Renderer();
  g_app = new App(g_renderer);
  g_renderer->setApp(g_app);

  // Start rendering
  g_renderer->start();

  // Start App
  g_app->start();
}

// ============================================================================
// Embind - Expose to JavaScript
// ============================================================================
EMSCRIPTEN_BINDINGS(framework) {
  function("startApp", &startApp);
  function("invokeCallback", &invokeCallback);
}
