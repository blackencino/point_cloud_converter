
cmake_minimum_required( VERSION 3.10.0 )

project( point_cloud_converter
    DESCRIPTION
        "Point cloud conversion utilities for OmniVerse stuff"
    LANGUAGES
        CXX
    VERSION
        0.0.1
)


message(STATUS "about to do conan cmake download")

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/release/0.15/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

message(STATUS "about to do conan cmake run")
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP
                BUILD missing)

add_subdirectory(third_party)

add_executable(e57_to_pts src/main.cpp)
target_link_libraries(e57_to_pts PUBLIC E57Format)


