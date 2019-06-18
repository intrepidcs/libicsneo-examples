# libicsneo C Example

This is an example console application that uses the icsneoc library to control an Intrepid Control Systems hardware device.

## Cloning
This will create a copy of the repository on your local machine.

Run:
```shell
git clone https://github.com/intrepidcs/libicsneo-examples --recursive
```

Alternatively, if you cloned without the --recursive flag, you must enter the libicsneo-examples folder and run the following:
```shell
git submodule update --recursive --init 
```
If you haven't done this, `third-party/libicsneo` will be empty and you won't be able to build!

## Building on Windows using Visual Studio

### Building the dll
First, we are going to build the icsneoc library into a .dll file that we can later use in order to access the library functions.
1. Launch Visual Studio and open the libicsneo-examples folder
2. Choose File->Open->Cmake...
3. Navigate to third-party/libicsneo and select the CMakeLists.txt there
4. Visual Studio will process the CMake project
5. Select Build->Rebuild All
6. Visual Studio will generate the icsneoc.dll file, which can then be found by selecting Project->Cmake Cache (x64-Debug Only)->Open in Explorer. If the file cannot be found, search in libicsneo-examples/third-party/libicsneo/out/build/x64-Debug and double-check that the build succeeded in step 5.
7. Move the icsneoc.dll file to the /C/Windows/System32 folder. This will allow it to be found by icsneo_init(), which loads all the library functions.
    * Alternatively, the icsneoc.dll file can be placed in the same directory as libicsneoc-example.exe, which is typically libicsneo-examples/libicsneoc-example/out/build/x64-Debug, although this is not recommended. For more information, see https://docs.microsoft.com/en-us/windows/desktop/dlls/dynamic-link-library-search-order.

### Building the example program
Although the example program will build without successfully completing the steps above, it will exit immediately upon running due to a failure to load any library functions.
1. Choose File->Open->Cmake...
2. Navigate to libicsneo-examples/libicsneoc-example and select the CMakeLists.txt there
3. Visual Studio will process the CMake project
4. Select Build->Rebuild All
5. Click on the dropdown arrow attached to the green play button (labelled "Select Startup Item") and select libicsneoc-example.exe
6. Click on the green play button to run the example

## Building on other operating systems

Unfortunately, the C API currently only supports Windows.
