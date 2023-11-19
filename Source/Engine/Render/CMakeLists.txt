cmake_minimum_required(VERSION 3.26)
project(RE-Render)

set(HEADER_DIR Public)
set(HEADER_FILES
        Public/Render/Renderer.h
)
set(SOURCE_FILES
        Private/Renderer.cpp
        Private/Renderer_Tick.cpp
)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIR} "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(${PROJECT_NAME} PUBLIC
        RE-RHI
)
