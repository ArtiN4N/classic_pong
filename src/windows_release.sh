x86_64-w64-mingw32-gcc main.c game.c input.c paddle.c ball.c timer.c -o classic_pong.exe -O1 -Wall -std=c99 -Wno-missing-braces -I include -L lib -lraylib -static-libgcc -lopengl32 -lgdi32 -lwinmm -static -lpthread

x86_64-w64-mingw32-gcc -o blah main.c game.c input.c paddle.c ball.c timer.c -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -s -O1 -D_DEFAULT_SOURCE -I/usr/local/include -isystem. -isystem/home/artinan/Documents/gameDev/classic_pong/src -isystem/home/artinan/Documents/gameDev/classic_pong/release/include -isystem/home/artinan/Documents/gameDev/classic_pong/src/external -L. -L/usr/local/lib -L/home/artinan/Documents/gameDev/classic_pong/src -lraylib -lm -lpthread -DPLATFORM_DESKTOP
x86_64-w64-mingw32-gcc -o test *.c -Iinclude -Llib -lraylib -lm -lpthread -lopengl32 -lgdi32 -lwinmm

zig cc -o test *.c -Iinclude -Llib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -target mipsel-linux-musl