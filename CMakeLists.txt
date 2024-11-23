cmake_minimum_required(VERSION 3.15)
project(project_name LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS"${CMAKE_CXX_FLAGS} -W -Werror -Wall -g -Wextra -O3 -pthread -fsanitize=address")
include_directories(src)
file(GLOB MAIN
	"src/*.cpp"
	"src/*.hpp"
	"src/*.h"
	)
add_executable(main ${MAIN})

