CC_FLAGS = -Wall -Wextra -pedantic -O0 -g -Wshadow -Wformat=2 \
	-Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 \
	-Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG \
	-D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address \
	-fsanitize=undefined -fno-sanitize-recover -fstack-protector
CC = g++ -std=c++17 $(CC_FLAGS)
CC17 = g++ -std=c++17 -O2

build/jury: jury.cpp
	$(CC) $^ -o $@

build/main: main.cpp
	$(CC) $^ -o $@

.PHONY: preview test
preview: build/main
	$^

test: build/jury build/main build/interactor