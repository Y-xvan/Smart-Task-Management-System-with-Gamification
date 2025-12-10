# build_cmake.ps1 - CMake Build Script for Windows

param(
    [switch]$Debug,
    [switch]$Clean,
    [string]$QtPath = "",
    [string]$Generator = "MinGW Makefiles"
)

# Clear any errors
$Error.Clear()

# Configuration
$BUILD_TYPE = if ($Debug) { "Debug" } else { "Release" }
$BUILD_DIR = "build"

Write-Host "=========================================" -ForegroundColor Green
Write-Host "Smart Task Management System - CMake Build" -ForegroundColor Green
Write-Host "=========================================" -ForegroundColor Green
Write-Host ""

# Clean build if requested
if ($Clean) {
    Write-Host "Cleaning previous build..." -ForegroundColor Yellow
    if (Test-Path $BUILD_DIR) {
        Remove-Item -Recurse -Force $BUILD_DIR
    }
}

# Create build directory
New-Item -ItemType Directory -Force -Path $BUILD_DIR | Out-Null

# Add Qt to PATH if specified
if ($QtPath -ne "") {
    Write-Host "Adding Qt to PATH: $QtPath" -ForegroundColor Cyan
    $env:PATH = "$QtPath;$env:PATH"
}

# Change to build directory
Push-Location $BUILD_DIR

try {
    # Configure with CMake
    Write-Host "Configuring CMake (Build Type: $BUILD_TYPE, Generator: $Generator)..." -ForegroundColor Green
    cmake -G $Generator -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }

    # Build
    Write-Host "Building project..." -ForegroundColor Green
    cmake --build . --config $BUILD_TYPE -j $env:NUMBER_OF_PROCESSORS
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }

    # Check results
    Write-Host "" 
    Write-Host "=========================================" -ForegroundColor Green
    Write-Host "Build completed successfully!" -ForegroundColor Green
    Write-Host "=========================================" -ForegroundColor Green
    Write-Host ""

    if (Test-Path "bin\task_manager.exe") {
        $fileSize = (Get-Item "bin\task_manager.exe").Length / 1KB
        Write-Host "✓ Console executable: bin\task_manager.exe [$($fileSize.ToString('F2')) KB]" -ForegroundColor Green
    }

    if (Test-Path "bin\task_manager_gui.exe") {
        $fileSize = (Get-Item "bin\task_manager_gui.exe").Length / 1KB
        Write-Host "✓ GUI executable: bin\task_manager_gui.exe [$($fileSize.ToString('F2')) KB]" -ForegroundColor Green
    } else {
        Write-Host "⚠ GUI executable not built (Qt may not be available)" -ForegroundColor Yellow
    }

    # Copy SQLite DLL if exists
    if (Test-Path "..\sqlite\sqlite3.dll") {
        Write-Host "Copying SQLite3 DLL..." -ForegroundColor Cyan
        Copy-Item "..\sqlite\sqlite3.dll" "bin\" -Force
    }

    Write-Host ""
    Write-Host "To run console version:" -ForegroundColor Yellow
    Write-Host "  cd build\bin && .\task_manager.exe" -ForegroundColor White
    Write-Host ""
    Write-Host "To run GUI version:" -ForegroundColor Yellow
    Write-Host "  cd build\bin && .\task_manager_gui.exe" -ForegroundColor White
    Write-Host ""

} catch {
    Write-Host "Error: $_" -ForegroundColor Red
    Pop-Location
    exit 1
}

# Return to original directory
Pop-Location

Write-Host "Build script completed successfully!" -ForegroundColor Green
