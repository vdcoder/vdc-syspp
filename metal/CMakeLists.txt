cmake_minimum_required(VERSION 3.20)
project(MetalServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Debug build by default, with debug symbols
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Add debug symbols
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Find oatpp
find_package(oatpp 1.3.0 REQUIRED)

# Add source files
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/controller/ApiController.hpp
    src/AppComponent.hpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC src)

# Link oatpp
target_link_libraries(${PROJECT_NAME}
    PUBLIC oatpp::oatpp
)

# Enable all warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()
