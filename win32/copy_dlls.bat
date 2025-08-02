@echo off
setlocal

set DEST=%~1
echo Copying DLLs to %DEST%

copy /Y "C:\msys64\ucrt64\bin\libcurl-4.dll" "%DEST%\libcurl-4.dll"
copy /Y "C:\msys64\ucrt64\bin\libssl-3-x64.dll" "%DEST%\libssl-3-x64.dll"
copy /Y "C:\msys64\ucrt64\bin\libcrypto-3-x64.dll" "%DEST%\libcrypto-3-x64.dll"
copy /Y "C:\msys64\ucrt64\bin\libidn2-0.dll" "%DEST%\libidn2-0.dll"
copy /Y "C:\msys64\ucrt64\bin\zlib1.dll" "%DEST%\zlib1.dll"
copy /Y "C:\msys64\ucrt64\bin\libbrotlidec.dll" "%DEST%\libbrotlidec.dll"
copy /Y "C:\msys64\ucrt64\bin\libcares-2.dll" "%DEST%\libcares-2.dll"
copy /Y "C:\msys64\ucrt64\bin\libsodium-*.dll" "%DEST%\libsodium.dll"

copy /Y "%~dp0src\certs\cacert.pem" "%DEST%\cacert.pem"
