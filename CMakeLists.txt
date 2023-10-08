cmake_minimum_required(VERSION 3.10.0)
project(proje VERSION 0.1.0 LANGUAGES C CXX)

find_package(jsoncpp CONFIG REQUIRED)

include_directories(/home/ahmetdem/src/vcpkg/installed/x64-linux/include)
link_directories(/home/ahmetdem/src/vcpkg/installed/x64-linux/lib)

add_executable(main main.cc)

target_link_libraries(main PRIVATE JsonCpp::JsonCpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
