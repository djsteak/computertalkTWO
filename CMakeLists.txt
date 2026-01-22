cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
)
FetchContent_MakeAvailable(SFML)

add_executable(main
        src/main.cpp
        src/Client.cpp
        src/Server.cpp
        src/Network.cpp
        src/Game.cpp
        src/Object.cpp
)

target_link_libraries(main PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Network
)
