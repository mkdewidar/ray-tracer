SOURCE=`find . -name \*.cpp -or -name \*.h`

g++ $SOURCE -o ray-tracer -Wall -Wextra -std=c++17
