cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

# ----------------------------------------
# Global settings
# ----------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ----------------------------------------
# Dependencies
# ----------------------------------------
include(FetchContent)

# ---- SFML ----
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(SFML)

# ---- Standalone Asio ----
FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-30-2
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(asio)

# ----------------------------------------
# Executable
# ----------------------------------------
add_executable(main
        src/main.cpp
        src/Network.cpp
        src/Client.cpp
        src/Client.h
        src/player.cpp
        src/player.h
        src/object.cpp
        src/object.h
)

target_compile_features(main PRIVATE cxx_std_17)

# Asio needs threads
find_package(Threads REQUIRED)

# ----------------------------------------
# Includes & links
# ----------------------------------------
target_include_directories(main PRIVATE
        ${asio_SOURCE_DIR}/asio/include
)

target_link_libraries(main PRIVATE
        SFML::Graphics
        Threads::Threads
)

# ----------------------------------------
# Platform niceties
# ----------------------------------------
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY VS_STARTUP_PROJECT main
)
