linux:
	gcc -o classic_pong_linux *.c -O1 -Wall -std=c99 -Wno-missing-braces -I ../include/ -L ../lib/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

windows:
	gcc -o classic_pong_windows.exe *.c -O1 -Wall -std=c99 -Wno-missing-braces -I ../include/ -L ../lib/ ../lib/libraylibwindows.a -lopengl32 -lgdi32 -lwinmm