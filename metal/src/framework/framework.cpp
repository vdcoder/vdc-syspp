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
// Event System - Frame-scoped callback registry
// ============================================================================
using EventCallback = std::function<void()>;
using StringEventCallback = std::function<void(const std::string&)>;
    
std::vector<EventCallback> g_eventCallbacks;  // Cleared each frame
std::vector<StringEventCallback> g_stringEventCallbacks;  // For string events

// Register a callback for this frame and return its ID (vector index)
int registerEventCallback(EventCallback callback) {
  int id = g_eventCallbacks.size();
  g_eventCallbacks.push_back(std::move(callback));
  return id;
}

// Register a string callback for this frame and return its ID
int registerStringEventCallback(StringEventCallback callback) {
  int id = g_stringEventCallbacks.size();
  g_stringEventCallbacks.push_back(std::move(callback));
  return id;
}

// Call a registered callback from JavaScript
void invokeEventCallback(int id) {
  if (id >= 0 && id < static_cast<int>(g_eventCallbacks.size())) {
    g_eventCallbacks[id]();
  }
}

// Call a registered string callback with the string value
void invokeStringEventCallback(int id, const std::string& value) {
  if (id >= 0 && id < static_cast<int>(g_stringEventCallbacks.size())) {
    g_stringEventCallbacks[id](value);
  }
}

// Clear callbacks at the start of each frame
void clearFrameCallbacks() {
  g_eventCallbacks.clear();
  g_stringEventCallbacks.clear();
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
    // Clear callbacks from previous frame
    clearFrameCallbacks();
    
    // Generate new VNode tree (this will register new callbacks)
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
      window.set("invokeEventCallback", val::module_property("invokeEventCallback"));
      window.set("invokeStringEventCallback", val::module_property("invokeStringEventCallback"));
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


// ============================================================================
// Event Callback Helpers - Create event handler strings
// ============================================================================

// Generic callback with no event data
inline std::string Func(EventCallback callback) {
    int callbackId = registerEventCallback(callback);
    return "invokeEventCallback(" + std::to_string(callbackId) + ")";
}

// Input change callback - receives the input's value
inline std::string FuncInputChange(StringEventCallback callback) {
    int callbackId = registerStringEventCallback(callback);
    return "invokeStringEventCallback(" + std::to_string(callbackId) + ", this.value)";
}

// Future: Mouse event callback (placeholder for future implementation)
// inline std::string FuncMouseEvent(int eventId) {
//     return "invokeCallbackMouseEvent(" + std::to_string(eventId) + ")";
// }

// Future: Keyboard event callback (placeholder for future implementation)
// inline std::string FuncKeyboardEvent(int eventId) {
//     return "invokeCallbackKeyboardEvent(" + std::to_string(eventId) + ")";
// }

// Future: Input event callback (placeholder for future implementation)
// inline std::string FuncInputEvent(int eventId) {
//     return "invokeCallbackInputEvent(" + std::to_string(eventId) + ")";
// }




class MyComponent : public ComponentBase {
private:
  int itemId;
  
public:
  MyComponent(IInvalidator* invalidator, int id) 
    : ComponentBase(invalidator), itemId(id) {}

  virtual VNode render() {
    return div({{"style", "border: 1px solid #ccc; padding: 10px; margin: 5px;"}}, {
        p({}, {text("Component Instance #" + std::to_string(itemId))}),
        button({{"onclick", Func([this]() {
            val console = val::global("console");
            console.call<void>("log", val("Clicked item " + std::to_string(itemId)));
            invalidate();
        })}}, {text("Click Me!")})
    });
  }
};




class App : public AppBase {
private:
  // Simple state
  int counter = 0;
  String message{"Hello from C++ with String!"};

public:
  App(IInvalidator* invalidator) : AppBase(invalidator) {
    // No callback registration in constructor anymore!
  }

  virtual void start() {}

  // Render returns VNode tree
  virtual VNode render() {   
    return div({{"style", "font-family: sans-serif; padding: 20px;"}}, {
        h1({}, {text(message.std_str())}),
        p({}, {text("Counter: " + std::to_string(counter))}),
        button({{"onclick", Func([this]() {
            counter++;
            invalidate();
        })}}, {text("Increment")}),
        button({{"onclick", Func([this]() {
            counter = 0;
            invalidate();
        })}}, {text("Reset")}),
        input({
            {"type", "text"}, 
            {"placeholder", "Enter message"},
            {"value", message.std_str()},
            {"oninput", FuncInputChange([this](const std::string& value) {
                message = String(value.c_str());
                invalidate();
            })}
        }),
        h2({}, {text("Multiple Component Instances:")}),
        MyComponent(this, 1).render(),
        MyComponent(this, 2).render(),
        MyComponent(this, 3).render()
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
  function("invokeEventCallback", &invokeEventCallback);
  function("invokeStringEventCallback", &invokeStringEventCallback);
}
