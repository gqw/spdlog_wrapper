cmake_minimum_required (VERSION 3.6)

project(wlog)


SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)

aux_source_directory(${CMAKE_SOURCE_DIR}/example/src/  EXAMPLE_SRC_FILES)
aux_source_directory(${CMAKE_SOURCE_DIR}/example/src/test  EXAMPLE_SRC_FILES)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)


add_executable(${PROJECT_NAME}  ${EXAMPLE_SRC_FILES} )
target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_HOME_DIRECTORY}/third/
	PUBLIC  ${CMAKE_SOURCE_DIR}/include)


