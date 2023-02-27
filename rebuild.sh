mkdir build 
cd build
cmake ../ -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=1 
# -DLLVM_DIR=~/llvm/llvm15-0-6/lib/cmake/llvm
