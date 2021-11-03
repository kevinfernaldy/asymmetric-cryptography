cmake_minimum_required(VERSION 3.5)

project(AdvancedCryptography VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        src/ui/mainwindow.cpp
        src/ui/mainwindow.ui
        include/ui/mainwindow.h

        include/crypto/keygenerator.h
        include/crypto/rsa.h
        include/crypto/elgamal.h
        include/crypto/paillier.h
        include/crypto/ecc.h
        include/util.h
        src/crypto/keygenerator.cpp
        src/crypto/rsa.cpp
        src/crypto/elgamal.cpp
        src/crypto/paillier.cpp
        src/crypto/ecc.cpp
        src/util.cpp

        include/ui/rsawidget.h
        include/ui/elgamalwidget.h
        include/ui/paillierwidget.h
        include/ui/eccwidget.h
        src/ui/rsawidget.cpp
        src/ui/elgamalwidget.cpp
        src/ui/paillierwidget.cpp
        src/ui/eccwidget.cpp
        src/ui/rsawidget.ui
        src/ui/elgamalwidget.ui
        src/ui/paillierwidget.ui
        src/ui/eccwidget.ui

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AdvancedCryptography
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET AdvancedCryptography APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(AdvancedCryptography SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(AdvancedCryptography
            ${PROJECT_SOURCES}
        )
    endif()
endif()

find_package(GMP REQUIRED)

target_link_libraries(AdvancedCryptography PRIVATE Qt${QT_VERSION_MAJOR}::Widgets gmp)

set_target_properties(AdvancedCryptography PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AdvancedCryptography)
endif()
