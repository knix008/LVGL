# Test Case Updates

## Overview
Updated test cases to reflect the special character layout changes in the Chunjiin input system.

## Changes Made

### Special Character Layout Changes
1. **Button 0**: Changed from `~`^` to `~.^`
   - Now cycles through: `~` → `.` → `^`

2. **Button 7**: Changed from `-_.` to `-_`
   - Now cycles through: `-` → `_`

## New Test Cases Added

### 1. `test_special_char_button_0()`
**Purpose**: Verify button 0 in special mode cycles through `~.^` correctly

**Test Steps**:
- Initialize state and switch to special mode
- Press button 0 four times
- Verify character cycling: `~` → `.` → `^` → `~` (loops back)

**Expected Behavior**:
- First press adds `~` to buffer (cursor_pos increments)
- Second press replaces with `.` (cursor_pos stays same)
- Third press replaces with `^` (cursor_pos stays same)
- Fourth press cycles back to `~` (cursor_pos stays same)

### 2. `test_special_char_button_7()`
**Purpose**: Verify button 7 in special mode cycles through `-_` correctly

**Test Steps**:
- Initialize state and switch to special mode
- Press button 7 three times
- Verify character cycling: `-` → `_` → `-` (loops back)

**Expected Behavior**:
- First press adds `-` to buffer (cursor_pos increments)
- Second press replaces with `_` (cursor_pos stays same)
- Third press cycles back to `-` (cursor_pos stays same)

### 3. `test_special_char_all_buttons()`
**Purpose**: Verify all special character buttons (0-9) work correctly

**Test Steps**:
- Initialize state and switch to special mode
- Press each button 0-9 once
- Verify each button adds at least one character

**Expected Behavior**:
- All buttons should successfully add characters to the buffer
- cursor_pos should increment or stay same (but never decrease)

## Test Results

All tests pass successfully:
```
test_special_char_button_0 passed
test_special_char_button_7 passed
test_special_char_all_buttons passed
```

## Files Modified

1. **test/test_chunjiin_extra.c**
   - Added 3 new test functions
   - Updated main() to run new tests
   - Lines 244-315: New test implementations
   - Lines 335-337: Added to main() test runner

## Running the Tests

```bash
cd test
./run_test.sh
```

Or manually:
```bash
cd test
gcc -I.. -o test_chunjiin_extra test_chunjiin_extra.c ../chunjiin.c ../chunjiin_hangul.c
./test_chunjiin_extra
```

## Coverage

The new tests cover:
- ✅ Special character button 0 label and cycling (`~.^`)
- ✅ Special character button 7 label and cycling (`-_`)
- ✅ All special character buttons (0-9) functionality
- ✅ Mode switching to special mode
- ✅ Character replacement behavior (cycling)
- ✅ Cursor position management during cycling

## Related Changes

These tests correspond to the following code changes:
- **chunjiin.c**: Updated `special_texts[]` array
- **chunjiin_hangul.c**: Updated `special_make()` function cases 0 and 7
