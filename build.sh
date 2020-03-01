#!/bin/bash
python3 -m venv env
source env/bin/activate
pip install -r requirements.txt
mkdir -p build
conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default
(cd build && conan install ..)
(cd build && cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release)
(cd build && cmake --build .)