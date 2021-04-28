set BUILD_CONFIG=Release

mkdir build
cd build

cmake -G "MinGW Makefiles" ^
      -DCMAKE_BUILD_TYPE:STRING="%BUILD_CONFIG%" ^
      -DCMAKE_PREFIX_PATH="%PREFIX%" ^
      -DCMAKE_FIND_ROOT_PATH="%LIBRARY_PREFIX%" ^
      -DCMAKE_INSTALL_PREFIX:PATH="%LIBRARY_PREFIX%" ^
      -DBUILD_SHARED_LIBS:BOOL=ON ^
      -DCC3D_EXT_NAME:STRING=cc3dext ^
      -DPython3_EXECUTABLE=%PYTHON% ^
      "%SRC_DIR%/cc3dext/MaBoSS"
if errorlevel 1 exit 1

cmake --build . --config "%BUILD_CONFIG%" --target install
if errorlevel 1 exit 1
