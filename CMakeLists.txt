# Copyright (c) 2024, Pedro López-Cabanillas
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.19)

project(SFUpdateChecker VERSION 0.1 LANGUAGES CXX)

set(PR_VERSION "0.9.0")
set(PR_PROJECT "vmpk")
set(PR_DATETIME "2023-12-29T00:59:59Z")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
qt_standard_project_setup(REQUIRES 6.4)

add_subdirectory(library)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

qt_add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    libdrumstick-updchk
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PR_PROJECT=${PR_PROJECT}
    PR_VERSION=${PR_VERSION}
    PR_DATETIME=${PR_DATETIME}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rss.xml ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/best_release.json ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
