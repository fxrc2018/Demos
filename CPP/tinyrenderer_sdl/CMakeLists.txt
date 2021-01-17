cmake_minimum_required (VERSION 3.4.1)
project(main)
set(CMAKE_BUILD_TYPE "Debug")
add_executable(main main.cpp model.cpp tgaimage.cpp)
# target_include_directories(main "${PROJECT_SOURCE_DIR}/include")
target_include_directories(main PUBLIC
    "${PROJECT_SOURCE_DIR}/include")
target_link_directories(main PUBLIC 
    "${PROJECT_SOURCE_DIR}/lib")

target_link_libraries(main mingw32 SDL2main SDL2)