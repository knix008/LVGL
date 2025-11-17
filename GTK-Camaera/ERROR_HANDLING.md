# Error Handling Guide - GTK Webcam Viewer

This document describes how the GTK Webcam application handles various errors gracefully.

## Overview

The application is designed to fail gracefully with proper error messages instead of crashing. All errors are caught, logged, and the application continues running in a degraded state.

## Error Handling Mechanisms

### 1. Camera Initialization Errors

**What Happens**: If camera cannot be opened at startup
- Application still launches
- Status shows "Camera Not Available"
- Start button is disabled until camera is available
- Clear error messages in console

**Console Output**:
```
Error: Failed to open camera 0
Make sure:
  1. Camera device /dev/video0 exists
  2. You have permission to access it
  3. No other application is using the camera
```

**Solution**:
```bash
# Check if camera exists
ls -la /dev/video*

# Add user to video group
sudo usermod -a -G video $USER

# Log out and back in for changes to take effect
```

### 2. Camera Capture Errors

**What Happens**: If camera disconnects during streaming
- Capture thread detects up to 10 consecutive read failures
- Camera automatically stops
- UI button changes back to "Start Camera"
- Status shows "Camera Disconnected"
- Application remains open and responsive

**Error Detection**:
```cpp
// In camera.cpp - capture_frames()
error_count++;
if (error_count >= max_errors) {
    // Gracefully stop and notify
    is_running = false;
    is_active = false;
}
```

### 3. Frame Display Errors

**What Happens**: If frame conversion to pixbuf fails
- Invalid frame is skipped
- No UI updates for that frame
- Next frame is processed normally
- Application continues running

**Protected Code**:
```cpp
GdkPixbuf* pixbuf = mat_to_pixbuf(frame);
if (pixbuf != nullptr) {
    gtk_image_set_from_pixbuf(...);
    g_object_unref(pixbuf);
}
// If pixbuf is null, frame is simply skipped
```

### 4. Runtime Exceptions

**What Happens**: Any unexpected exception is caught
- Error message logged to console
- Camera operations stop gracefully
- UI updates to show error state
- Application window remains open

**Protected Sections**:
- `GTKApp::init()` - Initialization
- `GTKApp::refresh_frame()` - Frame display loop
- `GTKApp::toggle_camera()` - Camera control
- `Camera::open()` - Camera opening
- `Camera::capture_frames()` - Frame capture

## Error Messages

### Console Output During Runtime

```
Normal Operation:
Camera opened successfully: 0
Resolution: 640x480
FPS: 30
Camera started
FPS: 29.8
FPS: 30.2
Camera stopped

Camera Disconnection:
Warning: Failed to read frame from camera
Warning: Failed to read frame from camera
Error: Camera disconnected or no longer available (reached 10 consecutive errors)

Exception Handling:
Exception in capture thread: [error details]
```

### UI Status Messages

| Status | Meaning | Action |
|--------|---------|--------|
| Camera Idle | Camera available, not running | Click Start to begin |
| Camera Running | Actively streaming | Click Stop to pause |
| Camera Stopped | Stopped after streaming | Click Start to resume |
| Camera Disconnected | Camera unplugged/unavailable | Reconnect camera |
| Camera Not Available | Not detected at startup | Check permissions |
| Error - Check console | Unexpected error occurred | Check terminal output |

## Testing Error Scenarios

### Test 1: No Camera Connected
```bash
# Run with no camera
./gtk_webcam

# Expected: Window opens with "Camera Not Available" status
# Expected: Start button is disabled
```

### Test 2: Camera Disconnected During Streaming
```bash
# Start streaming, then unplug camera while running
./gtk_webcam
# Click Start Camera
# [Unplug USB camera]

# Expected: UI updates to show "Camera Disconnected"
# Expected: App remains responsive
# Expected: Can close window normally
```

### Test 3: Permission Issues
```bash
# Try running without video group membership
./gtk_webcam

# Expected: Clear error message about permissions
# Expected: Window opens with disabled camera
```

### Test 4: Multiple Camera Instances
```bash
# Try running two instances simultaneously
./gtk_webcam &
./gtk_webcam

# Expected: First instance works
# Expected: Second instance shows "Camera Not Available"
# Expected: Both instances close cleanly
```

## Code-Level Error Handling

### Try-Catch Blocks

All major operations are wrapped:

```cpp
try {
    // Operation
    if (!success) {
        // Detailed error message
        return false;
    }
} catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    // Graceful shutdown
}
```

### Error Counting

Camera thread implements error counting:
- Tracks consecutive read failures
- Resets on successful frame
- Stops after `max_errors` failures (default: 10)
- Provides gradual degradation instead of immediate crash

### Resource Cleanup

Proper cleanup on all error paths:

```cpp
// Camera resources
~Camera() { close(); }  // Cleanup on destruction

void Camera::stop() {
    is_running = false;
    if (capture_thread.joinable()) {
        capture_thread.join();  // Wait for thread
    }
    // Clear frame queue
}

// GTK resources
void GTKApp::cleanup() {
    if (refresh_timer) g_source_remove(timer);
    camera.close();
    if (window) gtk_widget_destroy(window);
}
```

## Debugging Error Issues

### Enable Verbose Output

Check console output for detailed information:
```bash
./gtk_webcam 2>&1 | tee output.log
```

### Check System Camera

```bash
# List cameras
ls -la /dev/video*

# Test with OpenCV
python3 << 'EOF'
import cv2
cap = cv2.VideoCapture(0)
if cap.isOpened():
    ret, frame = cap.read()
    print("Camera works: %dx%d" % (frame.shape[1], frame.shape[0]))
    cap.release()
EOF
```

### Check Permissions

```bash
# Current user's groups
groups

# Should include "video" group
sudo usermod -a -G video $USER

# Check device permissions
ls -la /dev/video0
```

## Summary

The application implements comprehensive error handling:
- ✅ Graceful degradation instead of crashes
- ✅ Clear error messages to console
- ✅ UI feedback for error conditions
- ✅ Proper resource cleanup
- ✅ Continues running after errors
- ✅ Easy debugging with console output

All errors are recoverable, and the application should never crash unexpectedly.
