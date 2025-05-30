cmake_minimum_required(VERSION 3.10)
project(MyGameSFMLGUI VERSION 1.0)

# Find SFML
find_package(SFML 2 COMPONENTS graphics window system REQUIRED)

# Add your source files
add_executable(${PROJECT_NAME} main.cpp)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)

# For C++17 or newer features if needed
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)