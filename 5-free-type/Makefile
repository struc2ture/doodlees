BIN = bin/ppm_atlas

run: $(BIN)
	lldb $(BIN) -o run -o q

debug: $(BIN)
	lldb $(BIN) -o run

bin/ppm_atlas:
	clang -g src/ppm_atlas.c -o bin/ppm_atlas -I/usr/local/include/freetype2 -L/usr/local/lib/ -lfreetype

bin/example:
	clang -g src/example.c -o bin/example -I/usr/local/include/freetype2 -L/usr/local/lib/ -lfreetype
