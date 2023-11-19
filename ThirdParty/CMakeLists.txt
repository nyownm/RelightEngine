
# vulkan
add_library(vulkan INTERFACE)
set(VULKAN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vulkan/include)
target_sources(vulkan INTERFACE ${VULKAN_INCLUDE_DIR}/vulkan/vulkan.h)
target_include_directories(vulkan INTERFACE ${VULKAN_INCLUDE_DIR})

target_compile_definitions(vulkan INTERFACE VK_NO_PROTOTYPES)

if(ANDROID)
    target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_ANDROID_KHR)
elseif(WIN32)
    target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_WIN32_KHR)
elseif(APPLE)
    target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_METAL_EXT)
elseif(UNIX)
    # Choose WSI based on RE_WSI_SELECTION
    if(RE_WSI_SELECTION STREQUAL XCB OR RE_WSI_SELECTION STREQUAL XLIB OR RE_WSI_SELECTION STREQUAL WAYLAND)
        find_package(PkgConfig REQUIRED)
    endif()
    if(RE_WSI_SELECTION STREQUAL XCB)
        pkg_check_modules(XCB xcb REQUIRED)
        if(XCB_FOUND)
            target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_XCB_KHR)
        endif()
    elseif(RE_WSI_SELECTION STREQUAL XLIB)
        pkg_check_modules(X11 x11 REQUIRED)
        if(X11_FOUND)
            target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_XLIB_KHR)
        endif()
    elseif(RE_WSI_SELECTION STREQUAL WAYLAND)
        pkg_check_modules(WAYLAND wayland-client REQUIRED)
        if(WAYLAND_FOUND)
            target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_WAYLAND_KHR)
        endif()
    elseif(RE_WSI_SELECTION STREQUAL D2D)
        set(DIRECT_TO_DISPLAY TRUE)
        set(DIRECT_TO_DISPLAY TRUE PARENT_SCOPE)
        target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_DISPLAY_KHR)
    else()
        message(FATAL_ERROR "Unknown WSI")
    endif()
endif()

# vma
add_library(vma INTERFACE)
set(VMA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vma/src)
target_sources(vma INTERFACE ${VMA_DIR}/vk_mem_alloc.h)
target_include_directories(vma INTERFACE ${VMA_DIR})
target_link_libraries(vma INTERFACE vulkan)

# glm
add_library(glm INTERFACE)
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glm)
target_sources(glm INTERFACE ${GLM_DIR}/glm/glm.hpp)
target_include_directories(glm INTERFACE ${GLM_DIR})
target_compile_definitions(glm INTERFACE
        GLM_FORCE_SWIZZLE
        GLM_FORCE_RADIANS
        GLM_FORCE_CTOR_INIT
        GLM_ENABLE_EXPERIMENTAL
)

# tinygltf
add_library(tinygltf INTERFACE)
set(TINYGLTF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tinygltf)
target_sources(tinygltf INTERFACE ${TINYGLTF_DIR}/tiny_gltf.h ${TINYGLTF_DIR}/json.hpp)
target_include_directories(tinygltf INTERFACE ${TINYGLTF_DIR})

# libktx
set(KTX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ktx)

set(KTX_SOURCES
        ${KTX_DIR}/lib/checkheader.c
        ${KTX_DIR}/lib/dfdutils/createdfd.c
        ${KTX_DIR}/lib/dfdutils/colourspaces.c
        ${KTX_DIR}/lib/dfdutils/dfd.h
        ${KTX_DIR}/lib/dfdutils/dfd2vk.inl
        ${KTX_DIR}/lib/dfdutils/interpretdfd.c
        ${KTX_DIR}/lib/dfdutils/printdfd.c
        ${KTX_DIR}/lib/dfdutils/queries.c
        ${KTX_DIR}/lib/dfdutils/vk2dfd.c
        ${KTX_DIR}/lib/etcdec.cxx
        ${KTX_DIR}/lib/etcunpack.cxx
        ${KTX_DIR}/lib/filestream.c
        ${KTX_DIR}/lib/filestream.h
        ${KTX_DIR}/lib/formatsize.h
        ${KTX_DIR}/lib/hashlist.c
        ${KTX_DIR}/lib/info.c
        ${KTX_DIR}/lib/ktxint.h
        ${KTX_DIR}/lib/memstream.c
        ${KTX_DIR}/lib/memstream.h
        ${KTX_DIR}/lib/stream.h
        ${KTX_DIR}/lib/strings.c
        ${KTX_DIR}/lib/swap.c
        ${KTX_DIR}/lib/uthash.h
        ${KTX_DIR}/lib/texture.c
        ${KTX_DIR}/lib/texture.h

        # Basis Universal
        ${KTX_DIR}/lib/basis_sgd.h
        ${KTX_DIR}/lib/basis_transcode.cpp
        ${KTX_DIR}/lib/basisu/transcoder/basisu_containers.h
        ${KTX_DIR}/lib/basisu/transcoder/basisu_containers_impl.h
        ${KTX_DIR}/lib/basisu/transcoder/basisu_file_headers.h
        ${KTX_DIR}/lib/basisu/transcoder/basisu_global_selector_cb.h
        ${KTX_DIR}/lib/basisu/transcoder/basisu_global_selector_palette.h
        ${KTX_DIR}/lib/basisu/transcoder/basisu_transcoder_internal.h
        ${KTX_DIR}/lib/basisu/transcoder/basisu_transcoder_uastc.h
        ${KTX_DIR}/lib/basisu/transcoder/basisu_transcoder.cpp
        ${KTX_DIR}/lib/basisu/transcoder/basisu_transcoder.h
        ${KTX_DIR}/lib/basisu/transcoder/basisu.h
        ${KTX_DIR}/lib/basisu/zstd/zstd.c

        # KT1
        ${KTX_DIR}/lib/texture1.c
        ${KTX_DIR}/lib/texture1.h

        # KTX2
        ${KTX_DIR}/lib/texture2.c
        ${KTX_DIR}/lib/texture2.h

        # Vulkan support
        ${KTX_DIR}/lib/vk_format.h
        ${KTX_DIR}/lib/vkformat_check.c
        ${KTX_DIR}/lib/vkformat_enum.h
        ${KTX_DIR}/lib/vkformat_str.c
        ${KTX_DIR}/lib/vk_funcs.c
        ${KTX_DIR}/lib/vk_funcs.h
        ${KTX_DIR}/lib/vkloader.c
)

set(KTX_INCLUDE_DIRS
        ${KTX_DIR}/include
        ${KTX_DIR}/lib
        ${KTX_DIR}/lib/basisu/zstd
        ${KTX_DIR}/lib/basisu/transcoder
        ${KTX_DIR}/other_include
)

add_library(ktx STATIC ${KTX_SOURCES})

target_compile_definitions(ktx PUBLIC LIBKTX)
if(WIN32)
    target_compile_definitions(ktx PUBLIC "KTX_API=__declspec(dllexport)")
endif()
target_compile_definitions(ktx PUBLIC KTX_FEATURE_WRITE=0)
target_compile_definitions(ktx PUBLIC BASISD_SUPPORT_KTX2_ZSTD=0)
target_compile_definitions(ktx PUBLIC BASISU_NO_ITERATOR_DEBUG_LEVEL)

target_include_directories(ktx PUBLIC ${KTX_INCLUDE_DIRS})

target_link_libraries(ktx PUBLIC vulkan)

set_target_properties(ktx PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)

# imgui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
set(IMGUI_FILES
        "${IMGUI_DIR}/imgui.cpp"
        "${IMGUI_DIR}/imgui_demo.cpp"
        "${IMGUI_DIR}/imgui_draw.cpp"
        "${IMGUI_DIR}/imgui_widgets.cpp"
        "${IMGUI_DIR}/imconfig.h"
        "${IMGUI_DIR}/imgui.h"
        "${IMGUI_DIR}/imgui_internal.h"
        "${IMGUI_DIR}/imstb_rectpack.h"
        "${IMGUI_DIR}/imstb_textedit.h"
        "${IMGUI_DIR}/imstb_truetype.h")

add_library(imgui STATIC ${IMGUI_FILES})

target_include_directories(imgui PUBLIC ${IMGUI_DIR})

set_target_properties(imgui PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)

# glslang
option(ENABLE_SPVREMAPPER OFF)
option(ENABLE_GLSLANG_BINARIES OFF)
set(ENABLE_HLSL ON)
option(ENABLE_OPT OFF)
option(BUILD_TESTING OFF)
option(BUILD_EXTERNAL OFF)

add_subdirectory(glslang)

if(NOT TARGET glslang-default-resource-limits)
    add_library(glslang-default-resource-limits
            glslang/StandAlone/ResourceLimits.cpp)

    set_target_properties(glslang-default-resource-limits PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)

    target_include_directories(glslang-default-resource-limits
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glslang)
endif()

if(NOT MSVC)
    target_compile_options(glslang PRIVATE
            "-Wno-logical-op-parentheses"
            "-Wno-unused-parameter")

    target_compile_options(SPIRV PRIVATE
            "-Wno-logical-op-parentheses"
            "-Wno-unused-parameter")
endif()

set_target_properties(glslang PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)
set_target_properties(OGLCompiler PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)
set_target_properties(OSDependent PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)
set_target_properties(SPIRV PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)

# spirv-cross
add_subdirectory(spirv-cross)

set_target_properties(spirv-cross PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)
set_target_properties(spirv-cross-core PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)
set_target_properties(spirv-cross-glsl PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)
set_target_properties(spirv-cross-cpp PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)
set_target_properties(spirv-cross-hlsl PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)
set_target_properties(spirv-cross-msl PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)
set_target_properties(spirv-cross-reflect PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)
set_target_properties(spirv-cross-util PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)

# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")


if(NOT ANDROID)
    if(NOT DIRECT_TO_DISPLAY)
        # GLFW
        option(GLFW_BUILD_DOCS OFF)
        option(GLFW_BUILD_TESTS OFF)
        option(GLFW_BUILD_EXAMPLES OFF)
        option(GLFW_BUILD_TESTS OFF)
        option(GLFW_INSTALL OFF)
        option(BUILD_SHARED_LIBS ON)
        if(RE_WSI_SELECTION STREQUAL WAYLAND)
            set(GLFW_USE_WAYLAND ON)
        endif()

        add_subdirectory(glfw)
    endif()
endif()

# spdlog
add_subdirectory(spdlog)

#robin-map
add_library(robin-map INTERFACE)
set(ROBIN_MAP_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/robin-map/include)
target_sources(robin-map INTERFACE
        ${ROBIN_MAP_HEADER_DIR}/tsl/robin_growth_policy.h
        ${ROBIN_MAP_HEADER_DIR}/tsl/robin_hash.h
        ${ROBIN_MAP_HEADER_DIR}/tsl/robin_map.h
        ${ROBIN_MAP_HEADER_DIR}/tsl/robin_set.h
)
target_include_directories(robin-map INTERFACE ${ROBIN_MAP_HEADER_DIR})