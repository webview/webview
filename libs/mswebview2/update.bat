@echo off
setlocal
set script_dir=%~dp0
set script_dir=%script_dir:~0,-1%

set desired_version=%~1
if "%desired_version%" == "" (
    echo Please specify a version or "latest".
    exit /b 1
)

set "dest_dir=%script_dir%\versions"

:temp_name_loop
    set temp_name_prefix=webview_temp_
    rem Generate a random number with at least 10 digits.
    set temp_name=%random%%random%%random%%random%%random%%random%%random%%random%%random%%random%
    set temp_name=%temp_name_prefix%%temp_name:~0,10%
    set "temp_dir=%temp%\%temp_name%"
    if exist "%temp_dir%" goto :temp_name_loop
    echo Working directory: %temp_dir%

:stage_1
    set "temp_download_dir=%temp_dir%\download"

    rem Skip if desired version is not "latest" and already exists.
    set "dest_desired_versioned_subdir=%dest_dir%\%desired_version%"
    if not "%desired_version%" == "latest" if exist "%dest_desired_versioned_subdir%" (
        set version=%desired_version%
        goto :stage_2
    )

    set "url=https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2"
    if not "%desired_version%" == "latest" set "url=%url%/%desired_version%"

    if not exist "%temp_download_dir%" (
        mkdir "%temp_download_dir%"
        echo Downloading WebView2 %desired_version%...
        curl -sSL "%url%" | tar -xf - -C "%temp_download_dir%" || exit /b
    )

    set "nuspec_path=%temp_download_dir%\Microsoft.Web.WebView2.nuspec"
    set "version_pattern=<version>[0-9.]*</version>"
    set version=
    for /F "usebackq tokens=3 delims=<>" %%a in (`findstr /R "%version_pattern%" "%nuspec_path%"`) do set version=%%a
    if "%version%" == "" (
        echo Error: Missing version. 2>&1
        exit /b 1
    )

:stage_2
    set "temp_stage_dir=%temp_dir%\stage"
    mkdir "%temp_stage_dir%" || exit /b

    set stage_versioned_subdir_path=%temp_stage_dir%\%version%
    if not exist "%temp_download_dir%" goto :stage_3
    echo Copying files to stage...
    mkdir "%stage_versioned_subdir_path%" || exit /b
    xcopy /Y "%temp_download_dir%\build\native\include\WebView2.h" "%stage_versioned_subdir_path%"\ > nul || exit /b
    xcopy /Y "%temp_download_dir%\LICENSE.txt" "%stage_versioned_subdir_path%"\ > nul || exit /b

:stage_3
    echo Generating redirection header...
    set "stage_gen_header_path=%temp_stage_dir%\auto.h"
    set no_wv2_guard_def=WEBVIEW_EMBEDDED_WEBVIEW2_H
    set use_wv2_def=WEBVIEW_USE_EMBEDDED_WEBVIEW2
    set no_wv2_def=WEBVIEW_NO_EMBEDDED_WEBVIEW2
    set wv2_header_path_def=WEBVIEW_EMBEDDED_WEBVIEW2_HEADER
    (
        echo #ifndef %no_wv2_guard_def%
        echo #define %no_wv2_guard_def%
        echo #ifndef %wv2_header_path_def%
        echo #define %wv2_header_path_def% "%version%/WebView2.h"
        echo #endif /* %wv2_header_path_def% */
        echo #include %wv2_header_path_def%
        echo #endif /* %no_wv2_guard_def% */
    ) > "%stage_gen_header_path%" || exit /b

:stage_4
    set "dest_subdir_path=%dest_dir%\%version%"
    if exist "%stage_versioned_subdir_path%" (
        if exist "%dest_subdir_path%" (
            echo Removing "%dest_subdir_path%"...
            rmdir /Q /S "%dest_subdir_path%"
        )
    )

    echo Copying files to "%dest_dir%"...
    if not exist "%dest_dir%" mkdir "%dest_dir%" || exit /b
    xcopy /S /Y "%temp_stage_dir%" "%dest_dir%"\ > nul || exit /b

:stage_5
    echo Cleaning up temporary files...
    rmdir /Q /S "%temp_dir%" || exit /b
