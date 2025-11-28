cmake_minimum_required(VERSION 3.16)

project(SmartTaskManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Qml 
    Quick 
    QuickControls2
)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Source files - existing C++ business logic
set(CORE_SOURCES
    src/database/databasemanager.cpp
    src/database/DAO/ProjectDAO.cpp
    src/database/DAO/TaskDAOImpl.cpp
    src/project/Project.cpp
    src/project/ProjectManager.cpp
    src/statistics/StatisticsAnalyzer.cpp
    src/gamification/XPSystem.cpp
    src/HeatmapVisualizer/HeatmapVisualizer.cpp
    src/task/task.cpp
    src/task/TaskManager.cpp
)

# Qt/QML bridge sources (new)
set(QT_BRIDGE_SOURCES
    src/qt/main_qt.cpp
    src/qt/models/TaskListModel.cpp
    src/qt/models/ProjectListModel.cpp
    src/qt/models/ChallengeListModel.cpp
    src/qt/controllers/TaskController.cpp
    src/qt/controllers/GamificationController.cpp
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/src/qt
)

# QML Resources
qt_add_qml_module(${PROJECT_NAME}
    URI SmartTaskManager
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/TaskCard.qml
        qml/components/GamificationHUD.qml
        qml/components/NavigationBar.qml
        qml/components/ProgressBar.qml
        qml/components/CelebrationEffect.qml
        qml/pages/TaskListPage.qml
        qml/pages/ProjectsPage.qml
        qml/pages/AchievementsPage.qml
        qml/pages/StatisticsPage.qml
        qml/pages/SettingsPage.qml
    RESOURCES
        qml/resources/qml.qrc
    SOURCES
        ${CORE_SOURCES}
        ${QT_BRIDGE_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    SQLite::SQLite3
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
