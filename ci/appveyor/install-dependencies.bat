if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2017" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
)
if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2015" (
    call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /x64
    call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64
)

set FART="%CD%\tools\fart\fart.exe"

echo
echo ******                		 ******
echo ****** Compiling Velocypack ******
echo ******                		 ******
echo

mkdir tmp_velo
cd tmp_velo

echo Get velocypack from git...

git clone https://github.com/arangodb/velocypack.git
cd velo*

echo Setting linker settings from /MT to /MD
echo "Fart location: %FART%"
echo "Curent directory: %CD%"
%FART% "%CD%\cmake\Modules\AR_CompilerSettings.cmake" MTd MDd
%FART% "%CD%\cmake\Modules\AR_CompilerSettings.cmake" MT MD

echo "Configuring..."
cmake -G"Visual Studio 15 2017" -DCMAKE_BUILD_TYPE=Release -DBuildTools=OFF -DBuildVelocyPackExamples=OFF -DBuildTests=OFF -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" .. -A x64 -S . -B build
echo "Building..."
cmake --build build --config %CONFIGURATION% --target install

REM echo
REM echo ******                         ******
REM echo ****** Compiling JSONARNAGO    ******
REM echo ******                         ******
REM echo
REM 
REM echo git clone jsonarango...
REM git clone https://bitbucket.org/gems4/jsonarango.git
REM cd jsonarango
REM 
REM echo "Configuring..."
REM cmake -G"Visual Studio 15 2017" -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" .. -A x64 -S . -B build
REM echo "Building..."
REM cmake --build build --config %CONFIGURATION% --target install
REM 
cd ..\..
echo "Curent directory: %CD%"
REM 
REM REM Housekeeping
REM rd /s /q tmp_velo
REM 