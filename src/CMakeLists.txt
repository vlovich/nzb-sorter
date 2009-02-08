find_package(Qt4)

set(QT_REQUIRED true)
set(QT_DONT_USE_QTGUI true)

include(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SORTER_SRCS NzbFile.cpp NzbParser.cpp main.cpp)

add_definitions(${QT_DEFINITIONS})

set(SORTER_BINARY nzb-sorter)
add_executable(${SORTER_BINARY} ${SORTER_SRCS})
target_link_libraries(${SORTER_BINARY} ${QT_LIBRARIES})
