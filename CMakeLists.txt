cmake_minimum_required(VERSION 3.16)

project(SmartTaskManagement VERSION 1.0 LANGUAGES CXX)

# C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/sqlite
    ${CMAKE_SOURCE_DIR}/src/ui
)

# SQLite library path
link_directories(${CMAKE_SOURCE_DIR}/sqlite)

# Common source files (shared between console and GUI)
set(COMMON_SOURCES
    src/database/databasemanager.cpp
    src/database/DAO/ProjectDAO.cpp
    src/database/DAO/TaskDAOImpl.cpp
    src/database/DAO/ReminderDAO.cpp
    src/database/DAO/AchievementDAO.cpp
    src/database/DAO/DAOFactory.cpp
    src/database/DAO/ExperienceDAO.cpp
    src/database/DAO/HeatmapVisualizerDao.cpp
    src/database/DAO/SettingsDAO.cpp
    src/database/DAO/StatisticsDAO.cpp
    src/project/Project.cpp
    src/project/ProjectManager.cpp
    src/statistics/StatisticsAnalyzer.cpp
    src/gamification/XPSystem.cpp
    src/HeatmapVisualizer/HeatmapVisualizer.cpp
    src/task/task.cpp
    src/task/TaskManager.cpp
    src/Pomodoro/pomodoro.cpp
    src/reminder/ReminderSystem.cpp
    src/achievement/AchievementManager.cpp
)

# ============================================
# Console Application (task_manager)
# ============================================
add_executable(task_manager
    src/main.cpp
    src/ui/UIManager.cpp
    ${COMMON_SOURCES}
)

# Link SQLite3
target_link_libraries(task_manager PRIVATE sqlite3)

# Platform-specific settings for console
if(WIN32)
    target_link_libraries(task_manager PRIVATE -static-libgcc -static-libstdc++)
endif()

# ============================================
# GUI Application (task_manager_gui)
# ============================================

# Try to find Qt6 first, then Qt5
find_package(Qt6 COMPONENTS Core Gui Qml Quick QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Core Gui Qml Quick QUIET)
endif()

# Only build GUI if Qt is found
if(Qt6_FOUND OR Qt5_FOUND)
    message(STATUS "Qt found - building GUI application")
    
    # Qt-specific settings
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    # GUI source files
    set(GUI_SOURCES
        src/main_gui.cpp
        src/ui/TaskModel.cpp
        src/ui/ProjectModel.cpp
        src/ui/ReminderModel.cpp
        src/ui/GameController.cpp
        ${COMMON_SOURCES}
    )

    # Qt resource files
    set(QML_RESOURCES
        resources/qml.qrc
    )

    # Create GUI executable
    if(WIN32)
        # WIN32 flag prevents console window on Windows
        add_executable(task_manager_gui WIN32
            ${GUI_SOURCES}
            ${QML_RESOURCES}
        )
    else()
        add_executable(task_manager_gui
            ${GUI_SOURCES}
            ${QML_RESOURCES}
        )
    endif()

    # Link Qt libraries
    if(Qt6_FOUND)
        target_link_libraries(task_manager_gui PRIVATE
            Qt6::Core
            Qt6::Gui
            Qt6::Qml
            Qt6::Quick
            sqlite3
        )
    else()
        target_link_libraries(task_manager_gui PRIVATE
            Qt5::Core
            Qt5::Gui
            Qt5::Qml
            Qt5::Quick
            sqlite3
        )
    endif()

    # Platform-specific settings for GUI
    if(WIN32)
        target_link_libraries(task_manager_gui PRIVATE -static-libgcc -static-libstdc++)
        
        # Set Windows subsystem to GUI
        set_target_properties(task_manager_gui PROPERTIES
            WIN32_EXECUTABLE ON
        )
    endif()

    # Install targets
    install(TARGETS task_manager_gui
        RUNTIME DESTINATION bin
    )

    # Install Qt DLLs on Windows
    if(WIN32 AND Qt6_FOUND)
        get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
        
        if(WINDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET task_manager_gui POST_BUILD
                COMMAND ${WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR}/resources/qml $<TARGET_FILE:task_manager_gui>
            )
        endif()
    elseif(WIN32 AND Qt5_FOUND)
        get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
        
        if(WINDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET task_manager_gui POST_BUILD
                COMMAND ${WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR}/resources/qml $<TARGET_FILE:task_manager_gui>
            )
        endif()
    endif()

else()
    message(WARNING "Qt not found - GUI application will not be built. Only console version will be available.")
endif()

# Install console target
install(TARGETS task_manager
    RUNTIME DESTINATION bin
)

# Install SQLite DLL on Windows
if(WIN32)
    if(EXISTS ${CMAKE_SOURCE_DIR}/sqlite/sqlite3.dll)
        install(FILES ${CMAKE_SOURCE_DIR}/sqlite/sqlite3.dll DESTINATION bin)
    endif()
endif()

# Print build information
message(STATUS "===========================================")
message(STATUS "Smart Task Management System - Build Info")
message(STATUS "===========================================")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Console Executable: task_manager")
if(Qt6_FOUND OR Qt5_FOUND)
    message(STATUS "GUI Executable: task_manager_gui")
    if(Qt6_FOUND)
        message(STATUS "Qt Version: ${Qt6_VERSION}")
    else()
        message(STATUS "Qt Version: ${Qt5_VERSION}")
    endif()
endif()
message(STATUS "===========================================")
