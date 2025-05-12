#!/bin/sh
set -xe
zig cc --target=wasm32-freestanding --no-standard-libraries -Wl,--import-symbols -Wl,--export=game_width -Wl,--export=game_height -Wl,--export=game_update -Wl,--export=game_render -Wl,--no-entry -o back.wasm back.c
