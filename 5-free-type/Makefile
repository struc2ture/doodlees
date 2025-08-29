run: bin/main
	lldb bin/main -o run -o q

bin/main:
	clang src/main.c -o bin/main -I/usr/local/include/freetype2 -L/usr/local/lib/ -lfreetype

bin/example:
	clang src/example.c -o bin/example -I/usr/local/include/freetype2 -L/usr/local/lib/ -lfreetype