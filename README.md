To build programm & tests (example for Windows MinGW)
```cmd
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```
It will generate dynamic libraries & executables.

Run tests with `ctest` or `cmake --build . --target run_tests`.  
You also can launch to unit-test directly: 
- `./logger_test.exe`
- `./hasher_test.exe`
- `./csvloader_test.exe`
- `./walker_test.exe`
