@echo off
call :RenameFiles "%~1"
goto :eof

:RenameFiles
if not exist "%~1\" (
  echo Target folder not found: %~1
  exit /b 1
)

goto :add_ext_end
echo.
for %%f in (*) do (
  rem for extensionless files
  rem if "%%~xf"=="" (
  rem for files with extension
  if not "%%~nxf"=="%%~nf" (
    ren "%%f" "%%f.bar"
  )
)
:add_ext_end

goto :rename_file_ext_end
echo.
rem there is change to cause an infinite loop: file.bar.bar.bar
rem for /r "%~1" %%f in (*.foo) do (
rem use 'dir' to list files and hand them to the for loop
rem 'delims=' to aoid the problem caused by filenames with spaces
for /f "delims=" %%f in ('dir /b /s "%~1\*.foo"') do (
  echo rename "%%f" to "%%~nf.bar"
  ren "%%f" "%%~nf.bar"
)
:rename_file_ext_end

rem goto :recurrsive_rename_file_ext_end
echo.
for /r "%~1" %%f in (*) do (
  rem echo %%f
  rem /i: case-insensitive
  if /i "%%~xf"==".foo" (
    echo rename "%%f" to "%%~nf.bar"
    ren "%%f" "%%~nf.bar"
  )
)
:recurrsive_rename_file_ext_end

exit /b