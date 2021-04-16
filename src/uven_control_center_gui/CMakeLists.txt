cmake_minimum_required(VERSION 2.8.3)
project(uven_control_center_gui)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -g")
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(catkin REQUIRED COMPONENTS roscpp roslint rqt_gui rqt_gui_cpp std_msgs sensor_msgs)

find_package(PkgConfig REQUIRED )
find_package(QWT REQUIRED)

# yaml-cpp
pkg_check_modules( yaml-cpp REQUIRED yaml-cpp)
include_directories(${yaml-cpp_INCLUDE_DIRS})

include_directories(include qcustomplot ${QWT_INCLUDE_DIR}
)

## System dependencies are found with CMake's conventions

if ("${qt_gui_cpp_USE_QT_MAJOR_VERSION} " STREQUAL "5 ")
    find_package(Qt5Widgets REQUIRED)
else ()
    find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
    include(${QT_USE_FILE})
endif ()

catkin_python_setup()

roslint_cpp()

catkin_package(INCLUDE_DIRS include LIBRARIES ${PROJECT_NAME}
        CATKIN_DEPENDS roscpp rqt_gui_cpp std_msgs sensor_msgs
        DEPENDS YAML_CPP)

include_directories(
        include/uven_control_center_gui/
        ${CMAKE_CURRENT_BINARY_DIR}/..
        ${catkin_INCLUDE_DIRS}
        ${VISP_INCLUDE_DIRS}
)

if ("${qt_gui_cpp_USE_QT_MAJOR_VERSION} " STREQUAL "5 ")
    qt5_wrap_cpp(uven_control_center_gui_MOCS
            include/uven_control_center_gui/uven_control_center_gui.hpp
            qcustomplot/qcustomplot.h)
    qt5_wrap_ui(uven_control_center_gui_UIS_H
            src/uven_control_center_gui.ui
            )
else ()
    qt4_wrap_cpp(uven_control_center_gui_MOCS
            uven_control_center_gui.hpp
            qcustomplot/qcustomplot.h)
    qt4_wrap_ui(uven_control_center_gui_UIS_H
            src/uven_control_center_gui.ui
            )
endif ()



add_library(uven_control_center_gui src/uven_control_center_gui.cpp
        ${uven_control_center_gui_MOCS} ${uven_control_center_gui_UIS_H}
        qcustomplot/qcustomplot.cpp )
target_link_libraries(uven_control_center_gui ${VISP_LIBRARIES} ${catkin_LIBRARIES} ${yaml-cpp_LIBRARIES} ${QWT_LIBRARIES} -ldl)

if ("${qt_gui_cpp_USE_QT_MAJOR_VERSION} " STREQUAL "5 ")
    target_link_libraries(uven_control_center_gui Qt5::Widgets)
else ()
    target_link_libraries(uven_control_center_gui ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
endif ()

#############
## Install ##
#############

install(TARGETS uven_control_center_gui
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        )

install(FILES
        package.xml
        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
        )
