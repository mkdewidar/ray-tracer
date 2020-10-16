SOURCE=`find . -name test_\*.cpp`

g++ $SOURCE -o test-ray-tracer -std=c++17

./test-ray-tracer $@
