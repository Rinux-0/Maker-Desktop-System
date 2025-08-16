@echo off
setlocal

REM 获取当前用户名
for /f "tokens=3 delims=\" %%A in ("%USERPROFILE%") do set "username=%%A"

REM 定义下载链接和文件名
set "url=https://repo.huaweicloud.com/python/3.11.4/python-3.11.4-amd64.exe"
set "filename=python-3.11.4-amd64.exe"
set "installdir=C:\Users\%username%\AppData\Local\Programs\Python\Python311"

if not exist "%installdir%" (
    mkdir "%installdir%"
)

if not exist "%installdir%\%filename%" (
    echo Downloading python3.11.4...
	powershell -Command "(New-Object System.Net.WebClient).DownloadFile('%url%', '%installdir%\%filename%')
)

echo Installing python3.11.4...
"%installdir%\%filename%" /passive InstallAllUsers=1 PrependPath=1 TargetDir="%installdir%"

:end
endlocal
