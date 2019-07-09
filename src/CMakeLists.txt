cmake_minimum_required(VERSION 3.13)
project(Level-Set-Visualizer)

set(CMAKE_CXX_STANDARD 11)


set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

###### Local DIR #####
set(QT_HOME "/Users/apple/Desktop/Qt5.6/5.6/clang_64")
set(ITK_DIR "/usr/local/lib/cmake/ITK-4.12")
set(VTK_DIR "/usr/local/lib/cmake/vtk-8.2")


### Qt
set(CMAKE_PREFIX_PATH ${QT_HOME})
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_INCLUDE_CURRENT_DIR on)
# AUTOMOC和自動生成ui_xxx.h有關！on了就先不要去用qt5_wrap_ui
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTOUIC on)
set(CMAKE_AUTORCC on)
# 根據所需尋找所需package
find_package(Qt5 REQUIRED Core Widgets UiTools Xml Network Qml Quick)

### ITK & VTK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
    find_package(VTK REQUIRED)
    include(${VTK_USE_FILE})
else()
    find_package(ItkVtkGlue REQUIRED)
    include(${ItkVtkGlue_USE_FILE})
    set(Glue ItkVtkGlue)
endif()


# https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake
FILE(GLOB GUIsources
        ${CMAKE_SOURCE_DIR}/GUI/*.h
        ${CMAKE_SOURCE_DIR}/GUI/*.cpp
        ${CMAKE_SOURCE_DIR}/GUI/*.ui
        )
FILE(GLOB Modelsources
        ${CMAKE_SOURCE_DIR}/Model/*.h
        ${CMAKE_SOURCE_DIR}/Model/*.cpp
        )
FILE(GLOB Logicsources
        ${CMAKE_SOURCE_DIR}/Logic/*.h
        ${CMAKE_SOURCE_DIR}/Logic/*.cpp
        )

add_executable(${PROJECT_NAME} main.cpp ${GUIsources} ${Modelsources} ${Logicsources})

include_directories(${CMAKE_SOURCE_DIR}/GUI/)
include_directories(${CMAKE_SOURCE_DIR}/Model/)
include_directories(${CMAKE_SOURCE_DIR}/Logic/)

target_link_libraries(${PROJECT_NAME} ${Glue} ${VTK_LIBRARIES} ${ITK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui Qt5::UiTools Qt5::Xml Qt5::Network Qt5::Qml Qt5::Quick)

FILE(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
FILE(COPY test.png DESTINATION ${CMAKE_BINARY_DIR})