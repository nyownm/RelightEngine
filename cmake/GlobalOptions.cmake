set(RE_VALIDATION_LAYERS ON CACHE BOOL "Enable validation layers for every application.")
set(RE_VULKAN_DEBUG ON CACHE BOOL "Enable VK_EXT_debug_utils or VK_EXT_debug_marker if supported.")
set(RE_VALIDATION_LAYERS_GPU_ASSISTED OFF CACHE BOOL "Enable GPU assisted validation layers for every application (implicitly enables VKB_VALIDATION_LAYERS).")
set(RE_VALIDATION_LAYERS_BEST_PRACTICES OFF CACHE BOOL "Enable best practices validation layers for every application (implicitly enables VKB_VALIDATION_LAYERS).")
set(RE_VALIDATION_LAYERS_SYNCHRONIZATION OFF CACHE BOOL "Enable synchronization validation layers for every application (implicitly enables VKB_VALIDATION_LAYERS).")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions($<$<CONFIG:Debug,RelWithDebInfo>:RE_DEVELOPMENT>)

string(LENGTH "${CMAKE_SOURCE_DIR}/" RE_ROOT_PATH_SIZE)
add_definitions(-DROOT_PATH_SIZE=${RE_ROOT_PATH_SIZE})

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

include(GenerateExportHeader)