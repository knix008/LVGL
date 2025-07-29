# Running DRM Applications in X11 Windows

This guide shows you how to run your DRM-based LVGL application in an X11 window using virtual framebuffer technology.

## Method 1: Virtual Framebuffer with VNC (Recommended)

### Step 1: Run the Application
```bash
cd Source
./run_drm_windowed.sh
```

### Step 2: View the Application
The script will start a VNC server. You can view the application using:

**Option A: VNC Viewer (if installed)**
```bash
vncviewer localhost:5900
```

**Option B: Web Browser**
- Open your web browser
- Go to: `http://localhost:5900`

**Option C: Install VNC Viewer**
```bash
sudo apt-get install tigervnc-viewer
vncviewer localhost:5900
```

## Method 2: Manual Setup

### Step 1: Start Virtual Framebuffer
```bash
Xvfb :99 -screen 0 1024x768x24 -ac +extension GLX +render -noreset &
```

### Step 2: Start VNC Server
```bash
export DISPLAY=:99
x11vnc -display :99 -nopw -listen localhost -xkb -ncache 10 -ncache_cr -forever &
```

### Step 3: Run Your Application
```bash
cd Source/build
sudo DISPLAY=:99 ./main
```

### Step 4: View with VNC
```bash
vncviewer localhost:5900
```

## Method 3: Switch Back to SDL (Simplest)

If you prefer a simpler approach, you can switch back to SDL which works natively with X11:

### Step 1: Update Configuration
Edit `Source/lvgl/lv_conf.h`:
```c
#define LV_USE_SDL              1
#define LV_USE_LINUX_DRM        0
#define LV_USE_EVDEV            0
```

### Step 2: Update main.c
Replace DRM initialization with SDL:
```c
lv_display_t * disp = lv_sdl_window_create(1024, 768);
lv_sdl_window_set_title(disp, "LVGL Application");

lv_indev_t * mouse = lv_sdl_mouse_create();
lv_indev_t * keyboard = lv_sdl_keyboard_create();
```

### Step 3: Update CMakeLists.txt
Switch back to SDL2 dependencies.

### Step 4: Build and Run
```bash
cd Source/build
make
./main  # No sudo needed
```

## Advantages of Each Method

### Virtual Framebuffer + DRM
- ✅ Full DRM performance benefits
- ✅ Hardware acceleration
- ✅ Low latency
- ❌ More complex setup
- ❌ Requires VNC viewer

### SDL + X11
- ✅ Simple setup
- ✅ Native X11 window
- ✅ Easy to use
- ❌ Some performance overhead
- ❌ Not true DRM

## Troubleshooting

### VNC Connection Issues
- Make sure port 5900 is not blocked
- Check if x11vnc is running: `ps aux | grep x11vnc`
- Try different VNC clients

### Virtual Framebuffer Issues
- Check if Xvfb is running: `ps aux | grep Xvfb`
- Try different display numbers: `:98`, `:100`, etc.
- Check permissions for /dev/dri devices

### Performance Issues
- Virtual framebuffer adds some overhead
- For best performance, use DRM directly (full-screen)
- Consider using SDL for development, DRM for production

## Quick Test

To quickly test if everything is working:

```bash
cd Source
./run_drm_windowed.sh
```

Then in another terminal:
```bash
vncviewer localhost:5900
```

You should see your LVGL application running in a VNC window! 