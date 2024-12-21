# Copyright (c) 2024, Pedro López-Cabanillas
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.19)

project(libdrumstick-updchk
    VERSION 0.1.0
    LANGUAGES CXX
)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

qt_standard_project_setup(REQUIRES 6.4)

set(PROJECT_SOURCES
    releasestable.cpp
    bestreleases.cpp
)

set(PROJECT_HEADERS
    releasedata.h
    releasestable.h
    bestreleases.h
)

qt_add_library(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

target_include_directories( ${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/library;${CMAKE_BINARY_DIR}/library>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>" )
set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PROJECT_HEADERS}")

if(STATIC_DRUMSTICK)
    add_library( ${PROJECT_NAME}-static ALIAS ${PROJECT_NAME})
endif()

if(STATIC_DRUMSTICK)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        STATIC_LIB ${PROJECT_NAME}
        EXPORT_NAME UPDCHK
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        EXPORT_NAME UPDCHK
    )
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt6::Core
    Qt6::Network
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
