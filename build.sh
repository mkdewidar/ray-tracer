SOURCE=`find . -name \*.cpp -and -not -name test_\*`

g++ $SOURCE -o ray-tracer -Wall -Wextra -std=c++17 $@
