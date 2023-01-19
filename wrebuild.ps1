New-Item -ItemType Directory -Force -Path build
cmake -B ./build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DLLVM_DIR=E:\LLVM-15-0-6\lib\cmake\llvm
