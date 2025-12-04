# build.ps1 - Smart Task Management System Build Script

# Clear any errors
$Error.Clear()

# Configuration
$CXX = "g++"
$CXXFLAGS = "-std=c++17 -Wall -Wextra -I./include -I./common -I./sqlite"
$LDFLAGS = "-L./sqlite -lsqlite3"
$TARGET = "bin\task_manager.exe"

# Source files
$SOURCES = @(
    "src\main.cpp",
    "src\database\databasemanager.cpp",
    "src\database\DAO\ProjectDAO.cpp",
    "src\database\DAO\AchievementDAO.cpp",
    "src\achievement\AchievementManager.cpp",
    "src\database\DAO\ReminderDAO.cpp",
    "src\reminder\ReminderSystem.cpp",
    "src\database\DAO\TaskDAOImpl.cpp",
    "src\project\Project.cpp",
    "src\project\ProjectManager.cpp",
    "src\statistics\StatisticsAnalyzer.cpp",
    "src\gamification\XPSystem.cpp",
    "src\HeatmapVisualizer\HeatmapVisualizer.cpp",
    "src\ui\UIManager.cpp",
    "src\task\task.cpp",
    "src\task\TaskManager.cpp",
    "src\Pomodoro\pomodoro.cpp"
)

# Create directories
Write-Host "Creating directories..." -ForegroundColor Green
 New-Item -ItemType Directory -Force -Path build, bin
 New-Item -ItemType Directory -Force -Path build\database, build\database\DAO, build\project, build\statistics, build\gamification, build\HeatmapVisualizer, build\ui, build\task, build\achievement, build\reminder, build\Pomodoro

# Compile each source file
Write-Host "Compiling source files..." -ForegroundColor Green
$OBJECTS = @()

foreach ($source in $SOURCES) {
    # Convert source path to object path
    $object = $source -replace '^src\\', 'build\' -replace '\.cpp$', '.o'
    $OBJECTS += $object
    
    # Ensure object directory exists
    $objectDir = Split-Path $object -Parent
    if (!(Test-Path $objectDir)) {
        New-Item -ItemType Directory -Force -Path $objectDir | Out-Null
    }
    
    # Compile
    Write-Host "  Compiling: $source -> $object"
    $compileArgs = $CXXFLAGS.Split(' ') + @("-c", $source, "-o", $object)
    
    try {
        & $CXX @compileArgs
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Error: Failed to compile $source" -ForegroundColor Red
            exit 1
        }
    } catch {
        Write-Host "Error: Compilation failed for $source" -ForegroundColor Red
        Write-Host "Exception: $_" -ForegroundColor Red
        exit 1
    }
}

# Link object files
Write-Host "Linking executable..." -ForegroundColor Green
try {
    & $CXX $OBJECTS -o $TARGET $LDFLAGS.Split(' ')
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Error: Linking failed" -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "Error: Linking exception" -ForegroundColor Red
    Write-Host "Exception: $_" -ForegroundColor Red
    exit 1
}

# Copy SQLite DLL if exists
if (Test-Path "sqlite\sqlite3.dll") {
    Write-Host "Copying SQLite3 DLL..." -ForegroundColor Green
    Copy-Item "sqlite\sqlite3.dll" "bin\" -Force -ErrorAction SilentlyContinue
}

Write-Host "`nBuild completed successfully!" -ForegroundColor Green
Write-Host "Executable: $TARGET" -ForegroundColor Yellow

# Check if executable exists
if (Test-Path $TARGET) {
    $fileSize = (Get-Item $TARGET).Length / 1KB
    Write-Host "File size: $($fileSize.ToString('F2')) KB" -ForegroundColor Cyan
} else {
    Write-Host "Warning: Executable not found at expected location" -ForegroundColor Yellow
}