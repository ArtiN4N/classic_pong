linux:
	gcc -o build/classic_pong_linux src/*.c -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -L lib/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

windows:
	gcc -o build/classic_pong_windows.exe src/*.c -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -L lib/ lib/libraylibwindows.a -lopengl32 -lgdi32 -lwinmm