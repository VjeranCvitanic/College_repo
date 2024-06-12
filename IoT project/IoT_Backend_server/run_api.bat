@echo off
:: Check for administrative privileges
openfiles >nul 2>&1
if %errorlevel% neq 0 (
    echo This script requires administrative privileges.
    echo Please right-click on the script and select "Run as administrator".
    pause
    exit /b
)

echo Building Docker image...
docker build -t my-fastapi-app .

if %errorlevel% neq 0 (
    echo Failed to build Docker image.
    pause
    exit /b
)

echo Running Docker container...
docker run -d -p 8000:8000 --name my-fastapi-container my-fastapi-app

if %errorlevel% neq 0 (
    echo Failed to run Docker container.
    pause
    exit /b
)

echo Docker container is running and FastAPI app should be accessible at http://localhost:8000
echo Press any key to stop the Docker container and exit.

:: Wait for any key press
pause

echo Stopping Docker container...
docker stop my-fastapi-container
docker rm my-fastapi-container

echo Docker container stopped.
pause
