FILES := $(wildcard src/*.c)


out/tel: $(FILES)
	gcc -g -std=c99 -Wall -Werror -Wno-error=unused-function -o out/tel $(FILES)

check-cpp:
	g++ -std=c++98 -Wall -Werror -Wno-error=unused-function -o out/tel $(FILES)

optimize:
	gcc -std=c99 -Wall -Werror -Wno-error=unused-function -O3 -o out/tel $(FILES)
