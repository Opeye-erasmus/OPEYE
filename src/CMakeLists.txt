project(opeye)
cmake_minimum_required(VERSION 2.8.4)

# The tracker library
FILE(GLOB_RECURSE TRACKER_SRC_LIST  tracker/*.cpp)
if (UNIX)
    add_definitions("-Wno-deprecated-declarations")
endif( UNIX)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_definitions("-g")
endif(CMAKE_BUILD_TYPE MATCHES DEBUG)

include("${CMAKE_SOURCE_DIR}/dlib/dlib/cmake")
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/opencv/build")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(lib/json/src/)
include_directories(lib/args/)
include_directories(tracker)
include_directories(tracker/src)

add_library(tracker ${TRACKER_SRC_LIST})
target_link_libraries(tracker ${OpenCV_LIBS} dlib)

# The tools executable:
FILE(GLOB_RECURSE SRC_LIST  tools/*.cpp)
include_directories(tools/src)
include_directories(tracker/src)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif(MSVC)
add_executable(tools ${SRC_LIST})
target_link_libraries(tools ${OpenCV_LIBS} dlib tracker)


project ( "magnify" )

# declaring files in your project
file(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/magnifier/*.cpp)
file(GLOB_RECURSE HDR_LIST ${CMAKE_CURRENT_LIST_DIR}/magnifier/*.h)
file(GLOB_RECURSE UI_LIST ${CMAKE_CURRENT_LIST_DIR}/magnifier/*.ui)
file(GLOB_RECURSE QRC_LIST ${CMAKE_CURRENT_LIST_DIR}/magnifier/*.qrc)


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(Qt5Core)
find_package(Qt5Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

QT5_ADD_RESOURCES(RESOURCES_CPP ${QRC_LIST})
QT5_WRAP_CPP(HEADERS_MOC ${HDR_LIST})
QT5_WRAP_UI(HEADERS_UI ${UI_LIST})


include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
include_directories(magnifier/src)
include_directories(tracker/src)

add_executable(magnify ${SRC_LIST} ${HEADERS_MOC} ${HEADERS_UI} ${RESOURCES_CPP})
target_link_libraries(magnify ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} dlib tracker Qt5::Widgets)

