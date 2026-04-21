cmake_minimum_required(VERSION 3.16)
project(QtSerialMonitor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts SerialPort)

add_executable(QtSerialMonitor
    src/main.cpp
    src/MainWindow.cpp
    src/ControlPanel.cpp
    src/ChartWidget.cpp
    src/LogPanel.cpp
    src/DataGenerator.cpp
    src/widgets/CardWidget.cpp
    src/widgets/SidebarNav.cpp
    resources/resources.qrc
)

target_include_directories(QtSerialMonitor PRIVATE src)

target_link_libraries(QtSerialMonitor PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::SerialPort
)
