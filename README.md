

https://github.com/user-attachments/assets/50f20333-15e1-4a00-8ae2-8e74e8b3dd1a



# Terminal-Style Audio Player

A lightweight desktop MP3 player with a vim-inspired keyboard interface, built in C with [raylib](https://www.raylib.com/) for rendering. It browses the current directory for music files, reads ID3 metadata with TagLib, extracts embedded album art with libid3tag, and renders a live discrete Fourier transform (DFT) visualizer of the currently playing track! Future plans: more visualization modes with different mathematical formulas, and more.

## version 0.1
1. removed global variables, generally ([frowned upon in C](https://www.forrestthewoods.com/blog/global-variables-are-evil-and-unsafe/))
    - used  [stdatomic.h](https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/stdatomic.h.html) to do so; in Raylib for the callback function, which is attached to the audio stream in order to get data from the stream, there is no option to pass in user/other data, only the buffer data/frames are passed through. To allow for a global variable to determine if the visualizer data is to be displayed and calculated, an atomic state variable is created to keep track of if the user is in the visualizer mode. This allows for the O(n^2) calculations only to be performed if in the visualizer mode.
2. use "typedef enum" for game states, curtailing magic numbers, and replacing the inefficient/naive string comparison for game states.
3. New file traversal method similar to ps3 ui, where the currently selected file remains in the same position on screen, with other files moving up/down
4. Add support for ogg, wav, on top of mp3

## TODO
1. [ ] incorporate the metadata/album cover retrieval code/headers into version 0.1 audio_player.c, + add in Now Playing section
2. [ ] retrieve APIC data from scratch without libraries
3. [ ] add shuffle
4. [ ] add more colors to the visualizer instead of solid red 

## Features

- **Vim-keybindings for navigation** — move around the UI using `h` / `j` / `k` / `l`
- **Directory browser** — lists files in the current working directory and lets you scroll through them.
- **MP3 playback** — play, pause, and resume tracks via raylib's audio streaming.
- **Tag display** — shows title and artist read from the file's ID3 tags (via TagLib).
- **Album art extraction** — pulls the embedded `APIC` cover image out of the MP3's ID3 tag and displays it in the "Now Playing" pane.
- **Live audio visualizer** — computes discrete Fourier transform (dFT) of the audio stream in real time and draws it as a bar spectrum.

## Controls

| Key | Context | Action |
|-----|---------|--------|
| `h` | Right pane | Move focus to the left navigation pane |
| `l` | Left pane | Move focus into the currently hovered section (right pane) |
| `j` | File browser | Move selection down one file |
| `k` | File browser | Move selection up one file |
| `j` | Left pane | Move hover down (Directory → Now Playing → Visualizer) |
| `k` | Left pane | Move hover up (Visualizer → Now Playing → Directory) |
| `Enter` | File browser | Load and play the selected file (if it's an `.mp3`) |
| `Space` | Anywhere | Toggle play / pause on the current track |
| `Esc` | Anywhere | Close the window |

The left pane has three sections: **Directory**, **Now Playing**, and **Visualizer**. Navigate into the directory listing with `h`, pick a track with `j` / `k`, and press `Enter` to play it.

## Dependencies

- [raylib](https://www.raylib.com/) — windowing, input, drawing, and audio streaming
- [TagLib](https://taglib.org/) (C bindings, `tag_c.h`) — reading ID3 title/artist tags
- [libid3tag](https://github.com/DarkAudax/libid3tag) — low-level ID3 frame access for album art extraction
- macOS `sips` command-line tool — used to convert extracted JPEG cover art to PNG 

### Installing dependencies (macOS via Homebrew)

```bash
brew install raylib taglib libid3tag
```

### Installing dependencies (Debian/Ubuntu)

```bash
sudo apt install libraylib-dev libtag1-dev libid3tag0-dev
```
> Note: on Linux you'll also need to replace the `sips` calls in `audio_player.c` and `get_album_cover.c` with an equivalent (e.g. ImageMagick's `convert`) 

## Building

Compile all sources together and link against raylib, TagLib's C bindings, and libid3tag:

```bash
gcc audio_player.c directory_rendering.c math_visualization.c \
    -o audio_player \
    -lraylib -ltag_c -lid3tag -lm
```

On some systems you may need additional raylib system frameworks/libraries (OpenGL, GLFW dependencies, etc.) — consult the [raylib build instructions](https://github.com/raysan5/raylib/wiki) for your platform.

## Running

Run the built binary from inside a directory containing your `.mp3` files:

```bash
cd ~/Music/some-album
/path/to/audio_player
```

The player scans the current working directory on startup and lists its contents in the file browser.

## Known Limitations

This is a work-in-progress hobby project. Some known rough edges:

- **macOS-only cover art conversion**: album art conversion from JPEG to PNG shells out to macOS's `sips` utility (`system("sips -s format png ...")`). On Linux or Windows this will silently fail and no cover art will be shown.
- **The DFT is a naive O(n²) implementation**, not an FFT, so the visualizer's per-frame cost scales quadratically with `BUFFER_SIZE`.


