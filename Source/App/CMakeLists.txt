cmake_minimum_required(VERSION 3.26)
project(RE-App)

set(HEADER_DIR Public)
set(HEADER_FILES
        Public/App/Platform.h
        Public/App/Window.h
        Public/App/InputEvents.h
)
set(SOURCE_FILES
        Private/Main.cpp
        Private/InputEvents.cpp
        Private/Platform.cpp
        Private/Window.cpp
)

if(WIN32)
    set(HEADER_FILES ${HEADER_FILES}
            Public/App/Win32/WindowsPlatform.h
            Public/App/Win32/GltfWindow.h
    )
    set(SOURCE_FILES ${SOURCE_FILES}
            Private/Win32/WindowsPlatform.cpp
            Private/Win32/GltfWindow.cpp
    )

    add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} ${SOURCE_FILES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
        RE-Core
        RE-RHI
        RE-Render
)

# Link platform specific libraries
if(ANDROID)
    # Import game-activity static lib inside the game-activity_static prefab module.
    find_package(game-activity REQUIRED CONFIG)
    target_link_libraries(${PROJECT_NAME} PUBLIC log android game-activity::game-activity_static)
else()
    if(DIRECT_TO_DISPLAY)
        target_link_libraries(${PROJECT_NAME} PRIVATE dl)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
    endif()
endif()