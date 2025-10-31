#ifndef ApiController_hpp
#define ApiController_hpp

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

/**
 * API Controller
 */
class ApiController : public oatpp::web::server::api::ApiController {
public:
  ApiController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
  
  ENDPOINT("GET", "/health", health) {
    auto dto = oatpp::Fields<oatpp::String>({
      {"status", "healthy"},
      {"timestamp", std::to_string(std::time(nullptr))}
    });
    return createDtoResponse(Status::CODE_200, dto);
  }
  
  ENDPOINT("GET", "/api/hello", hello,
           QUERY(String, name, "name", "World")) {
    auto dto = oatpp::Fields<oatpp::String>({
      {"message", "Hello, " + name + "!"},
      {"endpoint", "/api/hello"}
    });
    return createDtoResponse(Status::CODE_200, dto);
  }

};

#include OATPP_CODEGEN_END(ApiController)

#endif /* ApiController_hpp */
