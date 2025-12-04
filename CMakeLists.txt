cmake_minimum_required(VERSION 3.16)
project(SmartTaskManagement VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt 6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

# Collect existing source files from src/ (excluding main.cpp and main_gui.cpp)
file(GLOB_RECURSE BACKEND_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/database/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/task/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/project/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gamification/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/statistics/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/reminder/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/achievement/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Pomodoro/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/HeatmapVisualizer/*.cpp"
)

# Exclude UIManager.cpp from backend sources (CLI-specific)
list(FILTER BACKEND_SOURCES EXCLUDE REGEX ".*UIManager\\.cpp$")

# Exclude HeatmapVisualizerDao.cpp (has case-sensitivity issue in include)
list(FILTER BACKEND_SOURCES EXCLUDE REGEX ".*HeatmapVisualizerDao\\.cpp$")

# GUI-specific sources
set(GUI_SOURCES
    src/main_gui.cpp
    src/ui/TaskModel.cpp
    include/ui/TaskModel.h
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/database/DAO
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite
)

# Create the GUI executable target first
qt_add_executable(SmartTaskGUI
    ${GUI_SOURCES}
    ${BACKEND_SOURCES}
)

# Add QML module to the executable
qt_add_qml_module(SmartTaskGUI
    URI SmartTaskGUI
    VERSION 1.0
    QML_FILES
        resources/qml/Main.qml
        resources/qml/QuestBoard.qml
        resources/qml/QuestCard.qml
)

# Link Qt 6 libraries
target_link_libraries(SmartTaskGUI PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)

# Link SQLite3 - try to find system SQLite first, then fall back to local
find_package(SQLite3 QUIET)
if(SQLite3_FOUND)
    target_link_libraries(SmartTaskGUI PRIVATE SQLite::SQLite3)
else()
    # Use local SQLite library from sqlite/ directory
    target_link_directories(SmartTaskGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/sqlite)
    target_link_libraries(SmartTaskGUI PRIVATE sqlite3)
endif()

# Set executable properties
set_target_properties(SmartTaskGUI PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installation
install(TARGETS SmartTaskGUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
