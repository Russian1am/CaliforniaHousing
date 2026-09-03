set -e

cd build
cmake ..
cmake --build .
mv ./app ..
cd ..
./app