cmake_minimum_required(VERSION 3.5)

project(WifiScannerApp LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS  Charts REQUIRED)


message("Searching for Linux netlink library")

#-------------------------------------------------------------------------
message("Searching for Linux netlink library")

if (LibNL_FIND_REQUIRED)
        message("  (required)")
else (LibNL_FIND_REQUIRED)
        message("  (not required)")
endif (LibNL_FIND_REQUIRED)

find_path(LibNL_INCLUDE_DIR netlink/netlink.h
        /usr/include
        /usr/include/libnl3
        /usr/local/include
        /usr/local/include/libnl3
)

find_library(LibNL_LIBRARY NAMES nl nl-3)
find_library(LibNL_ROUTE_LIBRARY NAMES nl-route nl-route-3)
find_library(LibNL_NETFILTER_LIBRARY NAMES nl-nf nl-nf-3)
find_library(LibNL_GENL_LIBRARY NAMES nl-genl nl-genl-3)

if (LibNL_INCLUDE_DIR AND LibNL_LIBRARY)
        set(LibNL_FOUND TRUE)
endif (LibNL_INCLUDE_DIR AND LibNL_LIBRARY)

if (LibNL_FOUND)
        if (NOT LibNL_FIND_QUIETLY)
        set(LibNL_LIBRARIES ${LibNL_LIBRARY} ${LibNL_ROUTE_LIBRARY} ${LibNL_NETFILTER_LIBRARY} ${LibNL_GENL_LIBRARY})
        message("Found netlink libraries:  ${LibNL_LIBRARIES}")
        message("Found netlink includes: ${LibNL_INCLUDE_DIR}")
                endif (NOT LibNL_FIND_QUIETLY)
ELSE (LibNL_FOUND)
        if (LibNL_FIND_REQUIRED)
                message("Netlink version 3 development packages cannot be found.")
                message("In Debian/Ubuntu, they may be called:")
                message("libnl-3-dev libnl-genl-3dev libnl-nf-3-dev libnl-route-3-dev")
                message(FATAL_ERROR "Could not find netlink library.")
        endif (LibNL_FIND_REQUIRED)
endif (LibNL_FOUND)

include_directories(${LibNL_INCLUDE_DIR})
#-------------------------------------------------------------------------

set(PROJECT_SOURCES
        wifi_lib.cpp
        wifi_lib.h
        wifiScanner.cpp
        wifiScanner.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(WifiScannerApp
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(WifiScannerApp SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(WifiScannerApp
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(WifiScannerApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(WifiScannerApp PRIVATE Qt${QT_VERSION_MAJOR}::Charts)
target_link_libraries(WifiScannerApp PRIVATE Qt${QT_VERSION_MAJOR}::Gui)

target_link_libraries(WifiScannerApp PRIVATE ${LibNL_INCLUDE_DIR} -lnl-genl-3 -lnl-3)
