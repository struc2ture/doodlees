BIN = bin/ft_gl

CFLAGS = -g -I/usr/local/include/freetype2 -I../../shared/stb -I/opt/homebrew/include
CFLAGS += -DGL_SILENCE_DEPRECATION
CFLAGS += -Wall -Werror -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable
LFLAGS = -L/usr/local/lib/ -lfreetype
LFLAGS += -L/opt/homebrew/lib -lglfw -framework OpenGL

run: $(BIN)
	lldb $(BIN) -o run -o q

debug: $(BIN)
	lldb $(BIN) -o run

bin/ft_gl: bin/common.o bin/ppm_write.o
	clang src/ft_gl.c bin/common.o bin/ppm_write.o -o bin/ft_gl $(CFLAGS) $(LFLAGS)

bin/ppm_write.o: bin/common.o src/ppm_write.c src/ppm_write.h
	clang src/ppm_write.c -o bin/ppm_write.o -c $(CFLAGS)

bin/common.o: $(wildcard src/common/*)
	clang src/common/common.c -o bin/common.o -c $(CFLAGS)

bin/ppm_atlas:
	clang -g src/ppm_atlas.c -o bin/ppm_atlas -I/usr/local/include/freetype2 -L/usr/local/lib/ -lfreetype

bin/example:
	clang -g src/example.c -o bin/example -I/usr/local/include/freetype2 -L/usr/local/lib/ -lfreetype

