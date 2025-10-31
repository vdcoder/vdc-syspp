#include "./AppComponent.hpp"
#include "./controller/ApiController.hpp"
#include "./controller/StaticController.hpp"

#include "oatpp/network/Server.hpp"

#include <iostream>
#include <filesystem>

void run() {
  
  // Register Components
  AppComponent components;
  
  // Get Router component
  OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
  
  // Determine static files path
  std::string staticPath = "./static";
  const char* staticEnv = std::getenv("STATIC_PATH");
  if (staticEnv) {
    staticPath = staticEnv;
  }
  
  // Check if static files exist
  bool hasStaticFiles = std::filesystem::exists(staticPath + "/index.html");
  
  if (hasStaticFiles) {
    // Create and add ApiController first (so /health and /api/* take priority)
    auto apiController = std::make_shared<ApiController>();
    router->addController(apiController);
    
    // Create and add StaticController last (catches remaining routes)
    auto staticController = std::make_shared<StaticController>(staticPath);
    router->addController(staticController);
  } else {
    // No static files - just serve API
    auto apiController = std::make_shared<ApiController>();
    router->addController(apiController);
  }
  
  // Get connection handler component
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);
  
  // Get connection provider component
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);
  
  // Create server
  oatpp::network::Server server(connectionProvider, connectionHandler);
  
  // Get port info
  const char* portEnv = std::getenv("PORT");
  std::string port = portEnv ? portEnv : "8080";
  
  std::cout << "\n┌─────────────────────────────────────┐\n";
  std::cout << "│  🚀 Metal Server is Running!       │\n";
  std::cout << "├─────────────────────────────────────┤\n";
  std::cout << "│  Port: " << port << "                         │\n";
  std::cout << "│  Mode: " << (hasStaticFiles ? "Full-Stack          " : "API Only            ") << "         │\n";
  std::cout << "│  Endpoints:                         │\n";
  if (hasStaticFiles) {
    std::cout << "│    GET  /           (Client App)    │\n";
    std::cout << "│    GET  /{file}     (Static Files)  │\n";
  }
  std::cout << "│    GET  /health                     │\n";
  std::cout << "│    GET  /api/hello?name=<name>      │\n";
  std::cout << "└─────────────────────────────────────┘\n";
  
  if (!hasStaticFiles) {
    std::cout << "\n⚠️  Static files not found at: " << staticPath << "\n";
    std::cout << "   To build the client: cd metal/src/client && ./build.sh\n";
    std::cout << "   Then set STATIC_PATH or copy to ./static\n\n";
  } else {
    std::cout << "\n✅ Serving client from: " << staticPath << "\n\n";
  }
  
  // Run server
  server.run();
}

int main() {
  
  // Initialize oatpp Environment
  oatpp::base::Environment::init();
  
  // Run application
  run();
  
  // Destroy oatpp Environment
  oatpp::base::Environment::destroy();
  
  return 0;
}
