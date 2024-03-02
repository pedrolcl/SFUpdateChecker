cmake_minimum_required(VERSION 3.19)

project(libSFUpdateChecker VERSION 0.1 LANGUAGES CXX)

set(PR_VERSION "0.1.0")
set(PR_PROJECT "vmpk")
set(PR_DATETIME "2000-12-29T23:59:59Z")

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

qt_add_library(${PROJECT_NAME} STATIC
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PR_PROJECT=${PR_PROJECT}
    PR_VERSION=${PR_VERSION}
    PR_DATETIME=${PR_DATETIME}
)

target_include_directories( ${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/library;${CMAKE_BINARY_DIR}/library>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>" )
set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PROJECT_HEADERS}")
add_library( ${PROJECT_NAME}-static ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
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
