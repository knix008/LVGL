# Audio Player with LVGL

A modern audio player application built with LVGL (Light and Versatile Graphics Library) that supports MP3 and WAV file formats.

## Features

- **Modern GUI** - Beautiful, responsive interface optimized for 320x640 resolution
- **Audio Format Support** - Play MP3 and WAV audio files
- **Playback Controls** - Play, Pause, Stop, and Seek functionality
- **Volume Control** - Adjustable volume with visual feedback
- **Track Information** - Display title, artist, and album metadata
- **Auto Directory Scanning** - Automatically loads all audio files from `audio/` directory
- **Keyboard Shortcuts** - Quick controls via keyboard

## Display Resolution

- **Width**: 320 pixels
- **Height**: 640 pixels

## Project Structure

```
Audio/
├── audio_player           # Executable
├── main.c                 # Main application entry point
├── audio_player.c/h       # Audio backend (SDL2_mixer)
├── audio_gui.c/h          # GUI implementation
├── audio/                 # Audio files directory
│   ├── *.mp3             # MP3 audio files
│   └── *.wav             # WAV audio files
├── tests/                 # Test suite
│   ├── test_audio_player.c
│   ├── test_utils.c
│   ├── mock_gui.c
│   ├── Makefile
│   └── README.md
├── lvgl/                  # LVGL library
├── Makefile               # Build configuration
├── generate_test_audio.py # Test audio generator
└── README.md              # This file
```

## Requirements

### System Dependencies

#### Ubuntu/Debian
```bash
sudo apt-get install libsdl2-dev libsdl2-mixer-dev libfreetype6-dev
```

#### Fedora
```bash
sudo dnf install SDL2-devel SDL2_mixer-devel freetype-devel
```

#### macOS
```bash
brew install sdl2 sdl2_mixer freetype
```

### Build Dependencies

- GCC or compatible C compiler
- Make
- pkg-config

## Building

1. **First-time setup** (builds LVGL):
```bash
./setup.sh
```

2. **Build the audio player**:
```bash
make
```

3. **Generate test audio files** (optional):
```bash
python3 generate_test_audio.py
```

## Running

```bash
./audio_player
```

Or build and run in one command:
```bash
make run
```

## Usage

### Controls

#### Mouse Controls
- Click **Play/Pause** button to toggle playback
- Click **Stop** button to stop playback
- Drag the **progress slider** to seek through the track
- Drag the **volume slider** to adjust volume
- Click on a track in the **playlist** to load and play it

#### Keyboard Shortcuts
- **Q** or **ESC** - Quit application
- **SPACE** - Toggle play/pause

### Adding Music

Place your MP3 and WAV files in the `audio/` directory:

```bash
cp your_music_file.mp3 audio/
cp your_music_file.wav audio/
```

The application automatically scans the directory on startup and displays all audio files in the playlist.

## Testing

### Run all tests:
```bash
cd tests
make test
```

### Test Results:
- **18 total tests**
- **100% pass rate** ✅
- Tests cover: initialization, playback, file loading, state management, utilities

See `tests/README.md` for detailed testing documentation.

## GUI Layout (320x640)

```
┌─────────────────────────────┐
│    Track Information        │
│    - Title                  │
│    - Artist                 │
│    - Album                  │
├─────────────────────────────┤
│    Progress Bar             │
│    [00:00 ──────── 03:45]   │
├─────────────────────────────┤
│    [Play/Pause]  [Stop]     │
├─────────────────────────────┤
│    Volume Control           │
│    [🔊 ──────────── 70%]    │
├─────────────────────────────┤
│    Playlist                 │
│    • bass_100hz.wav         │
│    • beep_1000hz.wav        │
│    • chord_C_major.wav      │
│    • test_440hz_A4.wav      │
│    (auto-scanned from       │
│     audio/ directory)       │
└─────────────────────────────┘
```

## Audio Backend

The audio playback is handled by **SDL2_mixer**, which provides:

- MP3 support (via mpg123)
- WAV support (built-in)
- Volume control
- Playback control (play, pause, stop)
- Position tracking

## Features in Detail

### Auto Directory Scanning
- Scans `audio/` directory on startup
- Finds all `.mp3` and `.wav` files (case-insensitive)
- Automatically populates playlist
- No manual file list maintenance needed

### Supported Formats

**MP3**
- Codec: MPEG-1/2 Layer 3
- Support: Via SDL2_mixer with libmpg123

**WAV**
- Codec: PCM (uncompressed)
- Support: Native SDL2_mixer support

### Performance

- Display refresh rate: ~30 FPS (configurable in lv_conf.h)
- Audio sample rate: 44.1 kHz
- Audio channels: Stereo (2 channels)
- Buffer size: 2048 samples

## Troubleshooting

### SDL2_mixer not found
```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-mixer-dev

# Fedora
sudo dnf install SDL2_mixer-devel

# macOS
brew install sdl2_mixer
```

### No audio files in playlist
1. Make sure `audio/` directory exists
2. Add MP3 or WAV files to `audio/`
3. Restart the application

### No sound
- Check system audio is working
- Verify audio files are valid
- Try adjusting volume slider in app

### Build errors
- Ensure LVGL is built: `./setup.sh`
- Clean and rebuild: `make clean && make`
- Check all dependencies are installed

## Generating Test Audio

To create test audio files:

```bash
python3 generate_test_audio.py
```

This generates 6 test WAV files in the `audio/` directory:
- `test_440hz_A4.wav` - Standard A note (440Hz)
- `test_262hz_C4.wav` - Middle C (262Hz)
- `test_880hz_A5.wav` - High A (880Hz)
- `chord_C_major.wav` - C Major chord
- `beep_1000hz.wav` - 1kHz beep
- `bass_100hz.wav` - Low bass tone

## Configuration

- **Display size**: Edit `DISP_HOR_RES` and `DISP_VER_RES` in `main.c`
- **Audio directory**: Change `music_dir` in `audio_gui.c` (currently set to `"audio"`)
- **LVGL settings**: Edit `lv_conf.h`

## License

This project uses LVGL which is licensed under the MIT license.

## Acknowledgments

- **LVGL** - Light and Versatile Graphics Library
- **SDL2** - Simple DirectMedia Layer
- **SDL2_mixer** - Audio mixing library
- **FreeType** - Font rendering library
