#    Copyright 2009 Vitali Lovich
#
#    This file is part of nzb-sorter.
#
#    nzb-sorter is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    nzb-sorter is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with nzb-sorter.  If not, see <http://www.gnu.org/licenses/>.

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
