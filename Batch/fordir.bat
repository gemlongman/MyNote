set PluginsDir=F:\CodeAxis\CodeAxis\Plugins

dir %PluginsDir%

for /D %%i in (%PluginsDir%/*) do (
    REM dir full path
    echo %%i
    REM dir name
    echo %%~ni
    
)
pause