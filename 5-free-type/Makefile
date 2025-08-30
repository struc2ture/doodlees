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

bin/ft_gl:
	clang src/ft_gl.c -o bin/ft_gl  $(CFLAGS) $(LFLAGS)

bin/ppm_atlas:
	clang -g src/ppm_atlas.c -o bin/ppm_atlas -I/usr/local/include/freetype2 -L/usr/local/lib/ -lfreetype

bin/example:
	clang -g src/example.c -o bin/example -I/usr/local/include/freetype2 -L/usr/local/lib/ -lfreetype
