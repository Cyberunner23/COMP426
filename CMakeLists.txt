cmake_minimum_required(VERSION 3.0)


#------------------------------------------------------------------------------
# Hunter & Project
#------------------------------------------------------------------------------
include("cmake/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.214.tar.gz"
    SHA1 "e14bc153a7f16d6a5eeec845fb0283c8fad8c358"
)

project(COMP426)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#------------------------------------------------------------------------------
# Other dependencies
#------------------------------------------------------------------------------

find_package(OpenGL REQUIRED)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glut/include")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/bin/Release/Win32/glew32.dll ${CMAKE_CURRENT_BINARY_DIR}/glew32.dll COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glut/bin/freeglut.dll ${CMAKE_CURRENT_BINARY_DIR}/freeglut.dll COPYONLY)

#------------------------------------------------------------------------------
# Main files
#------------------------------------------------------------------------------

set(SRCS
		src/main.cpp
)

set(HEADERS
)


#------------------------------------------------------------------------------
# Executable
#------------------------------------------------------------------------------

add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${PROJECT_NAME}
	${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/lib/Release/Win32/glew32.lib
	${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glut/lib/freeglut.lib
)

