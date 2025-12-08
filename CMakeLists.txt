cmake_minimum_required(VERSION 3.10)
project(SmartTaskManagementSystem VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# SQLite3 configuration
set(SQLITE3_DIR ${CMAKE_SOURCE_DIR}/sqlite)

# Source files
set(SOURCES
    src/main.cpp
    src/database/databasemanager.cpp
    src/database/DAO/ProjectDAO.cpp
    src/database/DAO/TaskDAOImpl.cpp
    src/database/DAO/ReminderDAO.cpp
    src/database/DAO/AchievementDAO.cpp
    src/project/Project.cpp
    src/project/ProjectManager.cpp
    src/statistics/StatisticsAnalyzer.cpp
    src/gamification/XPSystem.cpp
    src/HeatmapVisualizer/HeatmapVisualizer.cpp
    src/ui/UIManager.cpp
    src/task/task.cpp
    src/task/TaskManager.cpp
    src/Pomodoro/pomodoro.cpp
    src/reminder/ReminderSystem.cpp
    src/achievement/AchievementManager.cpp
)

# Create executable
add_executable(task_manager ${SOURCES})

# Set include directories for target
target_include_directories(task_manager PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/common
    ${SQLITE3_DIR}
)

# Link directories and libraries
target_link_directories(task_manager PRIVATE ${SQLITE3_DIR})
target_link_libraries(task_manager PRIVATE sqlite3)

# Windows specific settings
if(WIN32)
    target_link_options(task_manager PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# Set output directory
set_target_properties(task_manager PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

# Copy SQLite DLL on Windows
if(WIN32 AND EXISTS ${SQLITE3_DIR}/sqlite3.dll)
    add_custom_command(TARGET task_manager POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SQLITE3_DIR}/sqlite3.dll"
        $<TARGET_FILE_DIR:task_manager>
        COMMENT "Copying SQLite3 DLL to output directory"
    )
endif()
