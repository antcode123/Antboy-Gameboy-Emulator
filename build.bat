if not exist "Build" mkdir "Build"
cmake -B "Build" -S . -G Ninja
cmake --build "Build"