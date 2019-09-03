cmake_minimum_required(VERSION 3.8.2)


#------------------------------------------------------------------------------
# Hunter & Project
#------------------------------------------------------------------------------
include("cmake/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.164.tar.gz"
    SHA1 "7bcf092561389dbd70edb3fc228126ae4605e10c"
)


project(dashboard)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(PkgConfig)
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(gstreamer-plugins-base-1.0 REQUIRED gstreamer-plugins-base-1.0)
pkg_check_modules(gstreamer-gl-1.0 REQUIRED gstreamer-gl-1.0)


hunter_add_package(glew)
find_package(glew CONFIG REQUIRED)

hunter_add_package(glfw)
find_package(glfw3 CONFIG REQUIRED)

hunter_add_package(glm)
find_package(glm CONFIG REQUIRED)





#------------------------------------------------------------------------------
# Other dependencies
#------------------------------------------------------------------------------

find_package(OpenGL REQUIRED)


#------------------------------------------------------------------------------
# Main files
#------------------------------------------------------------------------------

include_directories(src)
include_directories(external/imgui)
include_directories(external/readerwriterqueue)

set(SRCS
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_impl_glfw.cpp
    external/imgui/imgui_impl_opengl3.cpp
    external/imgui/imgui_widgets.cpp
    src/main.cpp
    src/GstPipeline.cpp
    src/widgets/GstreamerVideo.cpp
)

set(HEADERS
    external/imgui/imconfig.h
    external/imgui/imgui.h
    external/imgui/imgui_impl_glfw.h
    external/imgui/imgui_impl_opengl3.h
    external/imgui/imgui_internal.h
    external/imgui/imstb_rectpack.h
    external/imgui/imstb_textedit.h
    external/imgui/imstb_truetype.h
    external/readerwriterqueue/atomicops.h
    external/readerwriterqueue/readerwriterqueue.h
    src/UIWrappers.hpp
    src/GstPipeline.hpp
    src/widgets/GstreamerVideo.hpp
)


#------------------------------------------------------------------------------
# Executable
#------------------------------------------------------------------------------

add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${PROJECT_NAME}
    glew::glew
    glfw
    glm
    OpenGL::GL
#    PkgConfig::gstreamer-1.0
#    PkgConfig::gstreamer-video-1.0
#    PkgConfig::gstreamer-plugins-base-1.0
#    /usr/lib/gstreamer-1.0/libgstvideotestsrc.so
)


link_directories(${gstreamer-1.0_LIBRARY_DIRS})
link_directories(${gstreamer-plugins-base-1.0_LIBRARY_DIRS})

target_link_libraries(${PROJECT_NAME} ${gstreamer-1.0_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${gstreamer-plugins-base-1.0_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${gstreamer-gl-1.0_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${gstreamer-1.0_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${gstreamer-plugins-base-1.0_LIBDIR}/gstreamer-1.0/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${gstreamer-plugins-base-1.0_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${gstreamer-gl-1.0_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PUBLIC ${gstreamer-1.0_CFLAGS_OTHER})
target_compile_options(${PROJECT_NAME} PUBLIC ${gstreamer-plugins-base-1.0_CFLAGS_OTHER})
target_compile_options(${PROJECT_NAME} PUBLIC ${gstreamer-gl-1.0_CFLAGS_OTHER})