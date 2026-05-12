# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(YadroEXCEL C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Optional: Set build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Add compiler flags (optional)
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    # Enable warnings
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source files (add all your .c files here)
set(SOURCES
    main.c
    util.c
    # another_source.c
)

# Header files (optional, for IDE support)
set(HEADERS
    # config.h
    util.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries (if needed)
# target_link_libraries(${PROJECT_NAME} m)  # For math library
# target_link_libraries(${PROJECT_NAME} pthread)  # For pthread

# Set output directory for executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Optional: Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)