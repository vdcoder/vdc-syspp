#ifndef JSTRING_HPP
#define JSTRING_HPP

#include <emscripten/val.h>
#include <string>
#include <sstream>

/**
 * JString - A lightweight string wrapper that stores strings in JavaScript
 * 
 * Instead of duplicating strings in both WASM and JS memory, JString keeps
 * a single copy in JavaScript's string store and only holds an integer ID
 * in C++. This provides:
 * 
 * - Memory efficiency: 4 bytes in WASM vs full string
 * - No copy overhead when crossing WASM/JS boundary
 * - Automatic cleanup via RAII
 * - Fast string operations leveraging V8's optimizations
 */
class JString {
private:
  int id = -1;  // ID in JS string store (-1 = null/empty)
  
  // Access the global JS string store
  static emscripten::val getStore() {
    static emscripten::val store = emscripten::val::undefined();
    if (store.isUndefined()) {
      // Initialize the store on first access
      emscripten::val window = emscripten::val::global("window");
      
      // Check if store exists
      if (window["__jstringStore"].isUndefined()) {
        // Create the store in JavaScript
        window.set("__jstringStore", emscripten::val::object());
        window["__jstringStore"].set("strings", emscripten::val::array());
        window["__jstringStore"].set("freeIds", emscripten::val::array());
        window["__jstringStore"].set("nextId", 0);
      }
      
      store = window["__jstringStore"];
    }
    return store;
  }
  
  // Allocate a new ID in the store
  static int allocateId(const std::string& str) {
    auto store = getStore();
    auto freeIds = store["freeIds"];
    
    int id;
    if (freeIds["length"].as<int>() > 0) {
      // Reuse a freed ID
      id = freeIds.call<emscripten::val>("pop").as<int>();
    } else {
      // Allocate new ID
      id = store["nextId"].as<int>();
      store.set("nextId", id + 1);
    }
    
    // Store the string
    auto strings = store["strings"];
    strings.set(id, emscripten::val(str));
    
    return id;
  }
  
  // Free an ID back to the store
  static void freeId(int id) {
    if (id < 0) return;
    
    auto store = getStore();
    auto strings = store["strings"];
    auto freeIds = store["freeIds"];
    
    // Delete the string
    strings.delete_(id);
    
    // Add ID to free list
    freeIds.call<void>("push", id);
  }

public:
  // Constructors
  JString() : id(-1) {}
  
  JString(const char* str) {
    if (str && str[0] != '\0') {
      id = allocateId(str);
    }
  }
  
  JString(const std::string& str) {
    if (!str.empty()) {
      id = allocateId(str);
    }
  }
  
  JString(int value) {
    id = allocateId(std::to_string(value));
  }
  
  // Copy constructor
  JString(const JString& other) {
    if (other.id >= 0) {
      id = allocateId(other.toStdString());
    }
  }
  
  // Move constructor
  JString(JString&& other) noexcept : id(other.id) {
    other.id = -1;  // Take ownership
  }
  
  // Destructor - RAII cleanup
  ~JString() {
    freeId(id);
  }
  
  // Copy assignment
  JString& operator=(const JString& other) {
    if (this != &other) {
      freeId(id);
      if (other.id >= 0) {
        id = allocateId(other.toStdString());
      } else {
        id = -1;
      }
    }
    return *this;
  }
  
  // Move assignment
  JString& operator=(JString&& other) noexcept {
    if (this != &other) {
      freeId(id);
      id = other.id;
      other.id = -1;
    }
    return *this;
  }
  
  // Get the underlying JS string
  emscripten::val toJS() const {
    if (id < 0) return emscripten::val("");
    auto store = getStore();
    return store["strings"][id];
  }
  
  // Convert to std::string (when needed)
  std::string toStdString() const {
    if (id < 0) return "";
    return toJS().as<std::string>();
  }
  
  // Implicit conversion to std::string
  operator std::string() const {
    return toStdString();
  }
  
  // String concatenation
  JString operator+(const JString& other) const {
    return JString(toStdString() + other.toStdString());
  }
  
  JString operator+(const char* str) const {
    return JString(toStdString() + str);
  }
  
  JString operator+(const std::string& str) const {
    return JString(toStdString() + str);
  }
  
  // Comparison
  bool operator==(const JString& other) const {
    if (id == other.id) return true;
    if (id < 0 && other.id < 0) return true;
    return toStdString() == other.toStdString();
  }
  
  bool operator!=(const JString& other) const {
    return !(*this == other);
  }
  
  // Check if empty
  bool empty() const {
    return id < 0 || toStdString().empty();
  }
  
  // Get ID (for debugging)
  int getId() const { return id; }
  
  // Stream output
  friend std::ostream& operator<<(std::ostream& os, const JString& jstr) {
    os << jstr.toStdString();
    return os;
  }
};

// Helper for string concatenation from left side
inline JString operator+(const char* lhs, const JString& rhs) {
  return JString(std::string(lhs) + rhs.toStdString());
}

inline JString operator+(const std::string& lhs, const JString& rhs) {
  return JString(lhs + rhs.toStdString());
}

#endif // JSTRING_HPP
