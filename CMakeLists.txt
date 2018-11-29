cmake_minimum_required(VERSION 2.8)

project(ElabImage)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Open-cv
find_package(OpenCV REQUIRED)
if(OpenCV_INCLUDE_DIRS)
	include_directories(${OpenCV_INCLUDE_DIRECTORIES})
	link_directories(${Boost_LIBRARY_DIRS})
endif(OpenCV_INCLUDE_DIRS)

include_directories(${PROJECT_SOURCE_DIR}/Lab_Session_2)

add_executable(calib ${PROJECT_SOURCE_DIR}/calib/calib.cpp)
target_link_libraries(calib ${OpenCV_LIBS})

add_executable(Lab_Session_1 ${PROJECT_SOURCE_DIR}/Lab_Session_1/lab_1.cpp)
target_link_libraries(Lab_Session_1 ${OpenCV_LIBS})

add_executable(Lab_Session_2 
						${PROJECT_SOURCE_DIR}/Lab_session_2/functions.cpp 
						${PROJECT_SOURCE_DIR}/Lab_session_2/functions.h
						${PROJECT_SOURCE_DIR}/Lab_session_2/Lab_2.cpp )
target_link_libraries(Lab_Session_2 ${OpenCV_LIBS})


add_executable(Lab_Session_3 ${PROJECT_SOURCE_DIR}/Lab_session_3/Lab_3.cpp)
target_link_libraries(Lab_Session_3 ${OpenCV_LIBS})

add_executable(OpenCV_test ${PROJECT_SOURCE_DIR}/OpenCV_test/test.cpp)
target_link_libraries(OpenCV_test ${OpenCV_LIBS})

add_executable(Local_Feature ${PROJECT_SOURCE_DIR}/Lab_session_4/Local_Feature.cpp)
target_link_libraries(Local_Feature ${OpenCV_LIBS})
