cmake_minimum_required(VERSION 3.26)
project(RE-Core)

set(HEADER_DIR Public)
set(HEADER_FILES
        Public/Core/Logging.h
)
set(SOURCE_FILES
        Private/Logging.cpp
)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIR} "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(${PROJECT_NAME} PUBLIC
        spdlog
        robin-map
)

