cmake_minimum_required(VERSION 3.8)


#------------------------------------------------------------------------------
# Hunter & Project
#------------------------------------------------------------------------------
include("cmake/HunterGate.cmake")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.214.tar.gz"
    SHA1 "e14bc153a7f16d6a5eeec845fb0283c8fad8c358"
)

project(COMP426 LANGUAGES CXX)

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

#set(HEADERS
	#src/GLUtils.hpp src/BallUtils.hpp src/CLUtils.hpp)


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/BallLogic.cl ${CMAKE_CURRENT_BINARY_DIR}/BallLogic.cl COPYONLY)

#------------------------------------------------------------------------------
# Executable
#------------------------------------------------------------------------------

add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
	${OPENCL_LIBRARIES}
	glfw
	glm
)

