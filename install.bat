@echo off
mkdir build
cd build
cmake ..
cmake --build . --config Release -j 8
cd ..
pip install -r src\wrapper_python\requirements.txt
python src\wrapper_python\setup.py build_ext -j 8 --build-lib=bin\
