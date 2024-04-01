#
#   Copyright 2024 (C) Alexey Dynda
#    This file is part of uProfiler.
#
#   GNU General Public License Usage

#   Protocol Library is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.

#   Protocol Library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

#   You should have received a copy of the GNU General Public License
#   along with Protocol Library.  If not, see <http://www.gnu.org/licenses/>.

#   Commercial License Usage

#   Licensees holding valid commercial uProfiler licenses may use this file in
#   accordance with the commercial license agreement provided in accordance with
#   the terms contained in a written agreement between you and Alexey Dynda.
#   For further information contact via email on github account.
#

cmake_minimum_required (VERSION 3.5)

file(GLOB_RECURSE SOURCE_FILES *.cpp *.c)

if (NOT DEFINED COMPONENT_DIR)

    project (unit_test)

    add_executable(unit_test ${SOURCE_FILES})

    target_link_libraries(unit_test uprofiler)

    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
    find_package(CppUTest QUIET)
    if (NOT CppUTest_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_search_module(CPPUTEST cpputest REQUIRED)
        target_link_libraries(${PROJECT_NAME} ${CPPUTEST_LIBRARIES})
    else()
        target_link_libraries(${PROJECT_NAME} CppUTest::CppUTest)
    endif()


    add_custom_target(check unit_test)

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS ".")

endif()
