mkdir build
cd build
cmake ..
make -j 8
cd ..
pip install -r src/wrapper_python/requirements.txt
python3 src/wrapper_python/setup.py  build_ext -j 8 --build-lib="bin/"

