# modplayer  q is quit - space is play/stop n is next song.

# MOD Tracker Player with SDL2 and SDL_mixer

This is a simple MOD music player written in C++ using the SDL2 library and SDL_mixer for music playback. It allows you to play MOD music files (.mod) from the `music/` directory and provides an interactive GUI interface.

## Dependencies

Before compiling and running the program, make sure you have the following libraries and development packages installed on your system:

- g++
- libsdl2-dev
- libsdl2-mixer-dev
- libsdl2-ttf-dev
- libsdl2-image-dev

You can install these dependencies on Ubuntu or Debian-based systems using the following command:

```bash
sudo apt update
sudo apt install g++ libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-image-dev
g++ mod_player.cpp -o mod_player -lSDL2 -lSDL2_mixer -lSDL2_ttf -lSDL2_image
