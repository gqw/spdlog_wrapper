cmake_minimum_required (VERSION 3.6)

project(wlog)

option(LOGGER_LEVEL "Log level" 0)

# SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)

aux_source_directory(${CMAKE_SOURCE_DIR}/example/src/  EXAMPLE_SRC_FILES)
aux_source_directory(${CMAKE_SOURCE_DIR}/example/src/test  EXAMPLE_SRC_FILES)

if (WIN32)
	add_definitions(-DUNICODE -D_UNICODE -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING -DPROJECT_NAME="${PROJECT_NAME}" /std:c++17 /utf-8)
else()
	add_definitions(-g -O0 -Wall -DPROJECT_NAME="${PROJECT_NAME}" -std=c++17)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_LOG_CONSOLE")

add_executable(${PROJECT_NAME}  ${EXAMPLE_SRC_FILES} )
target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_HOME_DIRECTORY}/third/spdlog/include
	PUBLIC  ${CMAKE_SOURCE_DIR}/include)
if (UNIX)
	target_link_libraries(${PROJECT_NAME} pthread)
endif()


