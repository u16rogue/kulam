rm -rf ./.build
mkdir -p .build
cmake . -G Ninja -B .build/ -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_TOOLCHAIN_FILE=cmake/x86_64-w64-mingw32.cmake
