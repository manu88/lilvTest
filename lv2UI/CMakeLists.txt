cmake_minimum_required(VERSION 3.16)

project(lv2UI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(lv2UI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        pluginManager.h pluginManager.cpp

        plugindescription.h
        plugininstance.h plugininstance.cpp
        URIMap.h
        uimanager.h uimanager.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET lv2UI APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(lv2UI SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(lv2UI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(lv2UI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../UIHost/include/")

if(APPLE)
  target_include_directories(lv2UI PUBLIC "/opt/homebrew/include/lilv-0/")
  target_include_directories(lv2UI PUBLIC "/opt/homebrew/include/suil-0/")
  target_include_directories(lv2UI PUBLIC "/opt/homebrew/include/serd-0/")
  target_include_directories(lv2UI PUBLIC "/opt/homebrew/include/")

  target_link_directories(lv2UI PUBLIC "/opt/homebrew/lib/")

elseif(LINUX)
  target_include_directories(lv2UI PUBLIC "/usr/local/include/lilv-0/")
  target_include_directories(lv2UI PUBLIC "/usr/include/suil-0/")
  target_include_directories(lv2UI PUBLIC "/usr/include/serd-0/")

endif()


target_link_libraries(lv2UI PUBLIC lilv-0)
target_link_libraries(lv2UI PUBLIC suil-0)
target_link_libraries(lv2UI PUBLIC serd-0)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.lv2UI)
endif()
set_target_properties(lv2UI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS lv2UI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(lv2UI)
endif()
