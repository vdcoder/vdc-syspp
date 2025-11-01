#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/html5.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <optional>
#include "String.hpp"
#include "VNode.hpp"
#include "Diff.hpp"
#include "Patch.hpp"

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
    
    virtual VNode render() = 0;
    
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
  std::optional<VNode> oldVNode;
  EM_VAL rootElement = 0;
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
    // Generate new VNode tree
    VNode newVNode = app->render();

    if (!oldVNode) {
      // First render - create initial DOM
      val document = val::global("document");
      val root = document.call<val>("getElementById", val("app-root"));
      
      if (!root.isNull() && !root.isUndefined()) {
        // Clear existing content
        root.set("innerHTML", val(""));
        
        // Render new VNode tree
        rootElement = renderVNode(newVNode);
        
        // Append to root
        EM_VAL rootHandle = root.as_handle();
        dom_appendChild(rootHandle, rootElement);
        
        // Setup event listeners
        setupEventListeners();
      }
    } else {
      // Subsequent renders - diff and patch
      DiffNode diff = diffNodes(*oldVNode, newVNode);
      
      if (diff.hasChanges()) {
        patch(rootElement, diff);
      }
    }

    // Store new VNode for next diff
    oldVNode = std::move(newVNode);
  }

  void setupEventListeners() {
    // Set up one-time global event handler
    val window = val::global("window");
    if (window["__eventHandlerSetup"].isUndefined()) {
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

  virtual VNode render() {
    return div({{"style", "font-family: sans-serif; padding: 20px;"}}, {
      h1({}, {text("MyComponent")})
    });
  }
};

class App : public AppBase {
private:
  // Simple state
  int counter = 0;
  String message{"Hello from C++ with String!"};
  
  // Event handler IDs (integers now!)
  int incrementEventId;
  int resetEventId;
  int updateMessageEventId;

public:
  App(IInvalidator* invalidator) : AppBase(invalidator) {
    // Register event handlers with lambdas
    incrementEventId = registerCallback([this]() {

      std::string stdall;

      for (int i = 0; i < 1000000; ++i) {
        String s(("Test" + std::to_string(i)).c_str()); // Loop creates and destroys strings
        std::string stds = s.std_str();
        std::string stds1 = s.std_str();
        int l = s.length();
        stdall += stds + stds1 + std::to_string(l);
      }

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
        message = String(input["value"]);
        invalidate();
      }
    });
  }

  virtual void start() {}

  // Render returns VNode tree
  virtual VNode render() {
    return div({{"style", "font-family: sans-serif; padding: 20px;"}}, {
      h1({}, {text(message.std_str())}),
      p({}, {text("Counter: " + std::to_string(counter))}),
      button({{"onclick", Func(incrementEventId)}}, {text("Increment")}),
      button({{"onclick", Func(resetEventId)}}, {text("Reset")}),
      input({{"id", "message-input"}, {"type", "text"}, {"placeholder", "Enter message"}}),
      button({{"onclick", Func(updateMessageEventId)}}, {text("Update Message")}),
      MyComponent(this).render()
    });
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
