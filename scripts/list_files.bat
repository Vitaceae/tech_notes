@echo off
call :ListFiles "%~1"
goto :eof

:ListFiles
if not exist "%~1\" (
  echo Target folder not found: %~1
  exit /b 1
)

rem goto :list_files_end
echo.
echo Files found in %~f1:
for /r "%~1" %%f in (*) do (
  echo %%~nxf
)
:list_files_end

goto :list_full_path_end
echo.
echo Files found:
for /r "%~1" %%f in (*) do (
  echo %%f
)
:list_full_path_end

goto :redirect_to_file_end
for /r %%f in (*) do (
  echo %%~nxf
) > foo.txt
:redirect_to_file_end

exit /b