# IPC Architecture for GUI and Webcam Applications

This project now uses **Inter-Process Communication (IPC)** to separate the GUI and webcam processing applications, replacing the previous multi-threaded approach.

## Architecture Overview

### Two Separate Applications:

1. **GUI Application (`main`)**
   - LVGL-based user interface (same as before)
   - All existing tabs and functionality preserved
   - Only the OpenCV tab now uses IPC instead of threading
   - Receives messages from webcam application via Unix domain socket
   - Runs independently

2. **Webcam Application (`webcam_ipc_app`)**
   - OpenCV-based webcam processing
   - **Displays live video feed with detection rectangles**
   - Captures video frames and performs object detection
   - Shows FPS, frame count, and detection count on video
   - Sends results to GUI application via Unix domain socket
   - Runs as a separate process with its own video window

### Communication Method:
- **Unix Domain Socket** at `/tmp/opencv_gui_socket`
- **UDP-style communication** for reliability
- **Message-based protocol** with different message types

## Video Display Features

The webcam application now includes:

### Live Video Window:
- **Real-time video feed** from camera at **640x480 resolution**
- **Detection rectangles** drawn on detected objects
- **Color-coded detections** (Green for Person, Blue for Objects, Red for Unknown)
- **Confidence percentages** displayed on each detection
- **FPS counter** in real-time
- **Frame counter** showing total frames processed
- **Detection count** showing number of objects detected

### Simulation Mode:
- **Visual simulation window** when no camera is available
- **Simulated detections** with rectangles and labels
- **Status information** displayed on screen
- **Fixed 640x480 resolution** for consistency

### Controls:
- **ESC key** or **Q key** to stop the webcam application
- **Real-time display** with 30 FPS target
- **Automatic window management**

## Message Types

```c
#define IPC_MSG_DETECTION 1        // Object detection results
#define IPC_MSG_FRAME_PROCESSED 2  // Frame processing status
#define IPC_MSG_STATUS 3           // General status messages
#define IPC_MSG_ERROR 4            // Error messages
```

## What Changed

### Before (Multi-threaded):
- GUI and webcam processing ran in the same process
- Used `pthread` to run webcam processing in background
- Webcam processing was integrated into the GUI application
- Single executable with internal threading
- No visual video display

### After (IPC-based):
- GUI and webcam processing run as separate processes
- GUI application creates a Unix domain socket
- Webcam application connects to the socket and sends messages
- **Webcam application shows live video with detections**
- Two separate executables communicating via IPC

## Building the Applications

### Quick Build:
```bash
cd Source
./build_ipc_apps.sh
```

### Manual Build:
```bash
cd Source
mkdir -p build
cd build
cmake ..
make main -j$(nproc)
make webcam_ipc_app -j$(nproc)
```

## Running the Applications

### Option 1: Run Both Together
```bash
cd Source
./run_ipc_apps.sh
```

### Option 2: Test Video Display
```bash
cd Source
./test_video_display.sh
```

### Option 3: Run Separately

**Terminal 1 - Start GUI:**
```bash
cd Source/build
./main
```

**Terminal 2 - Start Webcam:**
```bash
cd Source/build
./webcam_ipc_app
```

### Option 4: Run Webcam with Custom Model
```bash
cd Source/build
./webcam_ipc_app --model /path/to/your/model.onnx
```

## How It Works

1. **GUI Application starts first** and creates the Unix domain socket at `/tmp/opencv_gui_socket`
2. **OpenCV tab initializes** and starts an IPC receiver thread
3. **Webcam Application connects** to the socket and starts sending messages
4. **Webcam window opens** showing live video with detection rectangles
5. **Messages flow** from webcam to GUI via the socket
6. **GUI displays** the received information in the OpenCV tab
7. **Both applications** can be stopped independently

## File Changes

### Modified Files:
- `src/tab_opencv.c` - Added IPC socket functionality, removed threading
- `CMakeLists.txt` - Added webcam_ipc_app target

### New Files:
- `src/webcam_ipc_app.cpp` - Separate webcam application with video display
- `build_ipc_apps.sh` - Build script for both applications
- `run_ipc_apps.sh` - Run script for both applications
- `test_video_display.sh` - Test script for video display functionality

## Advantages of IPC Architecture

1. **Separation of Concerns**: GUI and webcam processing are completely separate
2. **Better Stability**: One application crashing doesn't affect the other
3. **Resource Isolation**: Each application has its own memory space
4. **Easier Debugging**: Can debug each application separately
5. **Independent Development**: Can modify webcam processing without affecting GUI
6. **Scalability**: Can run multiple webcam applications or add more features
7. **Visual Feedback**: Live video display with detection results

## GUI Application Features

The GUI application remains **exactly the same** as before, with all existing functionality:
- All tabs (DB, Settings, Number, Korean, ChunJiIn, QWERTY, Calendar, Clock, Video, OpenCV, Info)
- All input methods and features
- All existing UI components
- Only the OpenCV tab communication method changed

## Video Display Features

The webcam application now provides:

### Visual Elements:
- **Live video feed** from camera at **640x480 resolution**
- **Detection rectangles** with different colors per class
- **Confidence labels** showing detection accuracy
- **Performance metrics** (FPS, frame count, detection count)
- **Real-time updates** every frame
- **Fixed resolution** for consistent performance

### Detection Visualization:
- **Green rectangles** for Person detections
- **Blue rectangles** for Object detections  
- **Red rectangles** for Unknown detections
- **Confidence percentages** displayed above each rectangle
- **Multiple detections** supported simultaneously

### User Controls:
- **ESC key** to stop webcam
- **Q key** to stop webcam
- **Real-time interaction** with video window
- **Automatic window management**

## Troubleshooting

### GUI doesn't appear:
- Check if X11 display is available
- Try running in a different terminal
- Check for error messages

### Webcam window doesn't appear:
- Check if camera is available
- Try different camera indices
- Check for OpenCV window support
- Verify X11 forwarding if running remotely

### Webcam not connecting:
- Check if GUI is running first
- Verify socket file exists: `ls -la /tmp/opencv_gui_socket`
- Check for permission issues

### No camera found:
- The webcam app will run in simulation mode
- Check available cameras: `ls /dev/video*`
- Try different camera indices in the code

### Build errors:
- Make sure OpenCV is installed
- Check CMake configuration
- Verify all dependencies are available

## Future Enhancements

1. **Real YOLO Integration**: Replace simulation with actual YOLO detection
2. **Multiple Cameras**: Support for multiple webcam applications
3. **Configuration Files**: Load settings from config files
4. **Logging**: Add proper logging system
5. **Error Recovery**: Automatic reconnection on connection loss
6. **Control Messages**: Allow GUI to send control messages to webcam app
7. **Video Recording**: Save video with detections
8. **Advanced Visualization**: Add more visual elements and controls
