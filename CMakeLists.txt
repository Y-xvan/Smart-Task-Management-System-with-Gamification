cmake_minimum_required(VERSION 3.10)
project(SmartTaskManagementSystem VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/sqlite)

# Find SQLite3
link_directories(${CMAKE_SOURCE_DIR}/sqlite)

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

# Link libraries
target_link_libraries(task_manager sqlite3)

# Windows specific settings
if(WIN32)
    target_link_libraries(task_manager -static -static-libgcc -static-libstdc++)
endif()

# Set output directory
set_target_properties(task_manager PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

# Copy SQLite DLL on Windows
if(WIN32)
    add_custom_command(TARGET task_manager POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/sqlite/sqlite3.dll"
        $<TARGET_FILE_DIR:task_manager>
        COMMENT "Copying SQLite3 DLL to output directory"
    )
endif()
