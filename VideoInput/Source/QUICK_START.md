# Quick Start Guide - LVGL VideoInput Program

## 🚀 Quick Start (5 minutes)

### 1. Build the Program
```bash
cd Source/build
make main
```

### 2. Run the Program
```bash
./main
```

### 3. Use the Interface
- **Click tabs** to switch between features
- **Use mouse** to interact with buttons and controls
- **Press Ctrl+C** to exit

## 🎯 What You Can Do Right Now

### Video Playback
1. Click the **"Video"** tab
2. Watch the demo video play automatically
3. Use the control buttons to play/pause/stop

### OpenCV Functions
1. Click the **"OpenCV"** tab
2. Click **"Image Proc"** to see image processing demo
3. Click **"Camera"** to test camera access
4. Click **"Video"** to test video file processing

### Korean Input
1. Click the **"KOR"** tab for Korean text input
2. Click the **"CJI"** tab for ChunJiIn input method
3. Click the **"QWERTY"** tab for Korean keyboard

### Database
1. Click the **"DB"** tab to see encrypted database demo
2. View the SQLCipher encryption in action

### Settings
1. Click the **"Setting"** tab to see configuration options
2. View image displays (JPG, GIF, SVG)

## 🎮 Basic Controls

### Mouse Navigation
- **Left Click**: Select buttons and controls
- **Tab Switching**: Click tab names at the top
- **Button Interaction**: Click buttons to activate functions

### Keyboard Shortcuts
- **Ctrl+C**: Exit the program
- **Tab Navigation**: Use arrow keys (if supported)

## 📊 Tab Functions Summary

| Tab | Function | Quick Action |
|-----|----------|--------------|
| **DB** | Database | View encrypted data |
| **Setting** | Configuration | See settings and images |
| **NUM** | Numbers | Use numeric keypad |
| **KOR** | Korean Input | Type Korean text |
| **CJI** | ChunJiIn | Korean input method |
| **QWERTY** | Keyboard | Full keyboard layout |
| **CAL** | Calendar | View calendar |
| **Clock** | Time | See digital clock |
| **Video** | Video Playback | Watch videos |
| **Webcam** | Camera | Use webcam |
| **OpenCV** | Computer Vision | Image processing |
| **Info** | Information | Program details |

## 🔧 Quick Troubleshooting

### Program Won't Start
```bash
# Check if built correctly
ls -la main
# Rebuild if needed
make main
```

### No Video Playback
- Check if video files exist in `../assets/`
- Look for error messages in terminal

### Camera Not Working
- Check if camera is available: `ls /dev/video*`
- Look for camera error messages

### Korean Text Not Displaying
- Check if font file exists: `ls ../assets/NanumGothic-Regular.ttf`

### Database Issues
- SQLCipher is built from source, no system package needed

## 📝 Console Output

The program shows detailed information in the terminal:
- ✅ **Green messages**: Success
- ⚠️ **Yellow warnings**: Non-critical issues
- ❌ **Red errors**: Problems to fix

## 🎯 Next Steps

1. **Explore all tabs** to understand features
2. **Try video files** in the assets directory
3. **Test camera** if available
4. **Experiment with Korean input** methods
5. **Check OpenCV functions** for image processing

## 💡 Tips

- **Window Size**: The program opens in a 750x550 window
- **Performance**: Close other applications for better performance
- **Debugging**: Watch terminal output for detailed information
- **Exit**: Always use Ctrl+C to exit properly

---

**Need Help?** Check the full `HOW_TO_USE.md` guide for detailed instructions. 