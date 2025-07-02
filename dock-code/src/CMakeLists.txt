# Use GLOB_RECURSE to gather all source files under /src
FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.cpp)

list(APPEND EXTRA_COMPONENT_DIRS "./lib/")

# Register the main component with all gathered sources
idf_component_register(
    SRCS ${app_sources}
    INCLUDE_DIRS "." "main" "console" "safe" # Include root and subdirectories
)