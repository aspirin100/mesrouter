cmake_minimum_required(VERSION 4.1.1)

set(PROJECT_NAME mesrouter)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -Wall")

find_package(nlohmann_json 3.12.0 REQUIRED)

file(GLOB CPPS "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${CPPS})

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")