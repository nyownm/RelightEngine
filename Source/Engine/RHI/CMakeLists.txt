cmake_minimum_required(VERSION 3.26)
project(RE-RHI)

set(HEADER_DIR Public)
set(HEADER_FILES
        Public/RHI/RHI.h
        Public/RHI/VulkanLoader.h
)
set(SOURCE_FILES
        Private/RHI.cpp
        Private/VulkanLoader.cpp
)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIR} "${CMAKE_CURRENT_BINARY_DIR}")

if(${RE_VALIDATION_LAYERS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC RE_VALIDATION_LAYERS)
endif()

if(${RE_VULKAN_DEBUG})
    target_compile_definitions(${PROJECT_NAME} PUBLIC RE_VULKAN_DEBUG)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
        RE-Core
        vulkan
        ktx
        stb
        glm
        glslang
        SPIRV
        vma
        spirv-cross-glsl
        glslang-default-resource-limits
        spdlog
)

