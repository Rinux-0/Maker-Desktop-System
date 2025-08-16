@echo off

setlocal EnableDelayedExpansion

set "pythoncoreUser_key=HKEY_CURRENT_USER\SOFTWARE\Python\PythonCore"
set "pythoncoreLocal_key=HKEY_LOCAL_MACHINE\SOFTWARE\Python\PythonCore"
set "versions="
set "versionsExists=false"
set "pythonPath="
set "pythonPathWithoutExe="
set "pythonPathExists=false"
set "pythonVersion="
set "pythonVersionRight=false"
set "pythonEnvironmentPaths="
set "pythonEnvironmentScriptsPath="
set "pythonNeedVersion=false"
set "pythonRightPath=false"

reg query "%pythoncoreUser_key%" >nul 2>nul
if %errorlevel% equ 0 (
    for /f "tokens=*" %%i in ('reg query "%pythoncoreUser_key%" /f "*" /k ^| findstr /r /c:"[0-9]\.[0-9]"') do (
        set "version=%%i"
        for /f "delims=\ tokens=*" %%j in ("!version:%pythoncoreUser_key%\=!") do ( 
            set "versions=!versions! %%j"
			set "versionsExists=true"
        ) 	
    )
)

reg query "%pythoncoreLocal_key%" >nul 2>nul
if %errorlevel% equ 0 (
    for /f "tokens=*" %%i in ('reg query "%pythoncoreLocal_key%" /f "*" /k ^| findstr /r /c:"[0-9]\.[0-9]"') do (
        set "version=%%i"
        for /f "delims=\ tokens=*" %%j in ("!version:%pythoncoreLocal_key%\=!") do ( 
            set "versions=!versions! %%j"
			set "versionsExists=true"
        )
    )
)
if "!versionsExists!"=="false" (
    exit /b 1
)

for %%v in (%versions%) do (
    reg query "HKEY_CURRENT_USER\SOFTWARE\Python\PythonCore\%%v\InstallPath" >nul 2>nul
    if !errorlevel! equ 0 (
        for /f "tokens=2*" %%a in ('reg query "HKEY_CURRENT_USER\SOFTWARE\Python\PythonCore\%%v\InstallPath" ^| find "REG_SZ"') do (
			set "pythonPathWithoutExe=!pythonPathWithoutExe! %%b"
			set "pythonPathExists=true"
        )       
    )
	reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Python\PythonCore\%%v\InstallPath" >nul 2>nul
    if !errorlevel! equ 0 (
        for /f "tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Python\PythonCore\%%v\InstallPath" ^| find "REG_SZ"') do (
            set "pythonPathWithoutExe=!pythonPathWithoutExe! %%b"
			set "pythonPathExists=true"
        )       
    )
)
if "!pythonPathExists!"=="false" (
    exit /b 2
)

for %%v in (%pythonPathWithoutExe%) do (
    pushd %%v
    for /f "tokens=2 delims= " %%a in ('python --version 2^>^&1') do (
		set "pythonVersion=!pythonVersion! %%a"
		echo %%a | findstr /r "[0-9]" >nul
        if !errorlevel! equ 0 (
            set "pythonVersionRight=true"
        )
		if "%%a"=="3.11.4" (
			set "pythonNeedVersion=true"
		    pushd C:\
			for /f "tokens=2 delims= " %%b in ('python --version 2^>^&1') do (
                if "%%b" neq "3.11.4" (
					call :check_and_add_to_path "%%v;%%v\Scripts"	
					set "pythonRightPath=true"
                    exit /b 0				
                ) else (
                    exit /b 0
                )
            )
            call :check_and_add_to_path "%%v;%%v\Scripts"	
            set "pythonRightPath=true"
            popd
        )
    )    
    popd
)
if "!pythonVersionRight!"=="false" (
    exit /b 3
)
if "!pythonNeedVersion!"=="false" (
    exit /b 4
)
if "!pythonRightPath!"=="true" (
    exit /b 5
)

:check_and_add_to_path
set "pythonEnvironmentPath=%~1"

for /f "tokens=2,*" %%A in ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path') do (
    set "originalPath=%%B"
)
set "stringToCheck=python311"
for /f "tokens=1 delims=;" %%P in ("!originalPath!") do (
    set "firstPath=%%P"
    echo !firstPath! | findstr /i /c:"python311" >nul
    if !errorlevel! equ 0 (
        set "newPath=!originalPath!"
    ) else (
        set "newPath=%pythonEnvironmentPath%;!originalPath!"
    )
)

if "%newPath:~0,1%"==";" (
    set "newPath=%newPath:~1%"
)
reg add "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "%newPath%" /f

goto :eof
