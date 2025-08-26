CFLAGS = -g -I/opt/homebrew/include -I/usr/local/include -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I../../shared/stb
CFLAGS += -DGL_SILENCE_DEPRECATION
CFLAGS += -Wall -Werror -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable
LFLAGS = -L/opt/homebrew/lib -lglfw -framework OpenGL

IMGUI_DIR = ../../other/imgui
IMGUI_SRC = $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
IMGUI_SRC += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

run: bin/game
	lldb ./bin/game -o run

bin/game: src/main.cpp
	clang++ $(CFLAGS) $(LFLAGS) $< $(IMGUI_SRC) -o $@
