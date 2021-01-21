cmake_minimum_required (VERSION 3.6)

project(spdlog_wrapper)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)

file(GLOB SOURCE_FILES
	./src/*.h
	./src/*.cpp
)

if (WIN32)
	add_definitions(-DUNICODE -D_UNICODE -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING -DPROJECT_NAME="${PROJECT_NAME}" /std:c++latest /utf-8)
else()
	add_definitions(-g -O0 -Wall -std=c++2a)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_LOG_CONSOLE")

add_executable(${PROJECT_NAME}  ${SOURCE_FILES} )
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_HOME_DIRECTORY}/third/spdlog/include )
if (UNIX)
	target_link_libraries(${PROJECT_NAME} pthread)
endif()


