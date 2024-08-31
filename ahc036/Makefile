main: main.cc
	g++ -g -O0 -std=c++17 -o $@ $^

test: main
	./main < in.txt > out.txt