run: bin/main
	lldb bin/main -o run -o q

debug: bin/main
	lldb bin/main -o run

bin/main:
	clang -g src/main.c -o bin/main -I/usr/local/include/freetype2 -L/usr/local/lib/ -lfreetype

bin/example:
	clang -g src/example.c -o bin/example -I/usr/local/include/freetype2 -L/usr/local/lib/ -lfreetype