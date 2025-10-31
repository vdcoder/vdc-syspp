#ifndef StaticController_hpp
#define StaticController_hpp

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"

#include <fstream>
#include <sstream>
#include <vector>

#include OATPP_CODEGEN_BEGIN(ApiController)

/**
 * Static File Controller - Serves the WebAssembly client
 */
class StaticController : public oatpp::web::server::api::ApiController {
private:
  std::string m_staticPath;
  
  // Helper to read file content
  oatpp::String readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
      OATPP_LOGD("StaticController", "Failed to open file: %s", path.c_str());
      return nullptr;
    }
    
    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read into vector (handles binary data properly)
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
      OATPP_LOGD("StaticController", "Failed to read file: %s", path.c_str());
      return nullptr;
    }
    
    OATPP_LOGD("StaticController", "Read %ld bytes from: %s", size, path.c_str());
    
    // Create oatpp::String with data and size (copies data internally)
    return oatpp::String((const char*)buffer.data(), size);
  }
  
  // Get content type based on file extension
  oatpp::String getContentType(const oatpp::String& path) {
    if (path->find(".html") != std::string::npos) return "text/html";
    if (path->find(".js") != std::string::npos) return "application/javascript";
    if (path->find(".wasm") != std::string::npos) return "application/wasm";
    if (path->find(".css") != std::string::npos) return "text/css";
    if (path->find(".json") != std::string::npos) return "application/json";
    if (path->find(".png") != std::string::npos) return "image/png";
    if (path->find(".jpg") != std::string::npos || path->find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path->find(".svg") != std::string::npos) return "image/svg+xml";
    return "application/octet-stream";
  }

public:
  StaticController(const std::string& staticPath, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    : oatpp::web::server::api::ApiController(objectMapper), m_staticPath(staticPath)
  {}
  
  // Serve index.html at root
  ENDPOINT("GET", "/", root) {
    std::string indexPath = m_staticPath + "/index.html";
    OATPP_LOGD("StaticController", "Attempting to serve: %s", indexPath.c_str());
    
    auto content = readFile(indexPath);
    if (!content) {
      return createResponse(Status::CODE_404, "Client app not found. Please build it first with: cd metal/src/client && ./build.sh");
    }
    
    auto response = createResponse(Status::CODE_200, content);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }
  
  // Serve static files
  ENDPOINT("GET", "/{filename}", getFile,
           PATH(String, filename)) {
    // Security: prevent directory traversal
    if (filename->find("..") != std::string::npos) {
      return createResponse(Status::CODE_403, "Forbidden");
    }
    
    std::string fullPath = m_staticPath + "/" + filename->c_str();
    OATPP_LOGD("StaticController", "Attempting to serve: %s", fullPath.c_str());
    
    auto content = readFile(fullPath);
    
    if (!content) {
      std::string msg = "File not found: " + std::string(filename->c_str());
      return createResponse(Status::CODE_404, msg.c_str());
    }
    
    auto response = createResponse(Status::CODE_200, content);
    response->putHeader(Header::CONTENT_TYPE, getContentType(filename));
    
    // Add CORS headers for WebAssembly
    response->putHeader("Cross-Origin-Opener-Policy", "same-origin");
    response->putHeader("Cross-Origin-Embedder-Policy", "require-corp");
    
    return response;
  }
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* StaticController_hpp */
