cmake_minimum_required(VERSION 3.8)


#------------------------------------------------------------------------------
# Hunter & Project
#------------------------------------------------------------------------------
include("cmake/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.214.tar.gz"
    SHA1 "e14bc153a7f16d6a5eeec845fb0283c8fad8c358"
)

project(COMP426 LANGUAGES CXX)

set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

hunter_add_package(glfw)
find_package(glfw3 CONFIG REQUIRED)

hunter_add_package(glm)
find_package(glm REQUIRED)


#------------------------------------------------------------------------------
# Other dependencies
#------------------------------------------------------------------------------

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
include_directories(${OPENCL_INCLUDE_DIRS})


#------------------------------------------------------------------------------
# Main files
#------------------------------------------------------------------------------

set(SRCS
	src/main.cpp
)

set(HEADERS
	src/GLUtils.hpp src/BallUtils.hpp src/CLUtils.hpp)


#------------------------------------------------------------------------------
# Executable
#------------------------------------------------------------------------------

add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
	${OPENCL_LIBRARIES}
	glfw
	glm
)

