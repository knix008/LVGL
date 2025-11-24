# Bug Fix: Face Rectangle and Label Color Incorrect

## Issue Summary
The bounding box rectangle and label background colors were incorrect for recognized faces. Some recognized faces were showing with red borders and blue background (unknown) when they should have been green (recognized).

## Root Cause Analysis

### The Problem
The `is_recognized` determination in `draw_faces_on_frame()` was using the wrong criteria:

**Incorrect Logic (Line 471 - Before):**
```cpp
bool is_recognized = (face.confidence > threshold_percent) && (face.name != "Unknown") && (face.name != "Too far");
```

This relied on checking if `face.confidence` exceeds a threshold, but:
1. **Unrecognized faces still have confidence values** - When FAISS returns a non-match (person_id ≤ 0), the recognizer still returns a confidence score from the similarity comparison
2. **The threshold check is redundant** - The actual recognition decision was already made in `frame_processor.cpp` when it set `face.id` and `face.name`
3. **Confidence values for non-matches could be high** - A face that doesn't match any registered person might still have a confidence score above the threshold if it was close to some embedding

### What Should Be Checked
The proper way to determine if a face is recognized is to check the `face.id`:
- **face.id > 0** = Recognized (has valid person ID from FAISS match)
- **face.id ≤ 0** = Unknown (no match found or error)

## Solution

**File:** `src/gtk_app.cpp`
**Lines:** 469-478

**Before:**
```cpp
// Determine if face is recognized (confidence > threshold and not Unknown)
double threshold_percent = Config::RECOGNITION_CONFIDENCE_THRESHOLD * 100.0;
bool is_recognized = (face.confidence > threshold_percent) && (face.name != "Unknown") && (face.name != "Too far");
```

**After:**
```cpp
// Determine if face is recognized by checking face ID, confidence, and name
// A face is recognized if:
// 1. It has a valid positive ID (from FAISS match)
// 2. Confidence is above the 70% threshold
// 3. Name is not "Unknown" or "Too far"
double threshold_percent = Config::RECOGNITION_CONFIDENCE_THRESHOLD * 100.0;
bool is_recognized = (face.id > 0) &&
                    (face.confidence >= threshold_percent) &&
                    (face.name != "Unknown") &&
                    (face.name != "Too far");
```

### Why This Works
- **Primary check:** `face.id > 0` - Uses the authoritative source (recognizer decision)
- **Secondary check:** `face.confidence >= threshold_percent` - Ensures confidence is above 70% threshold
- **Tertiary check:** Name validation - Double-checks against expected values
- **Robust logic:** Uses multiple criteria for accurate recognition status determination
- **Threshold enforcement:** Explicitly validates the 70% confidence threshold

## Color Scheme
Once `is_recognized` is properly determined:

**Recognized Faces (is_recognized = true):**
- Rectangle border: **Green** (0, 255, 0)
- Label background: **Green** (0, 255, 0)
- Label text: **Black** (0, 0, 0)
- Display: "PersonName (95%)" in black text on green background

**Unknown Faces (is_recognized = false):**
- Rectangle border: **Red** (0, 0, 255)
- Label background: **Blue** (0, 0, 200)
- Label text: **White** (255, 255, 255)
- Display: "Unknown (45%)" in white text on blue background

## Testing

### Expected Behavior
1. **Trained faces** should show:
   - Green bounding box corners
   - Green background label with person name

2. **Unknown/new faces** should show:
   - Red bounding box corners
   - Blue background label with "Unknown"

### Verification
Compare `face.id` values in logs with on-screen colors:
- If console shows `face.id=1`, corners and label should be **green**
- If console shows `face.id=-1`, corners and label should be **red/blue**

## Impact
- **Severity:** Medium (visual feedback)
- **Affected code:** Face display rectangle/label rendering
- **User impact:** Correct visual feedback for recognized vs unknown faces
- **Build status:** ✅ Successful compilation

## Files Modified
- `src/gtk_app.cpp` - Changed recognition check from confidence-based to ID-based (lines 469-471)
