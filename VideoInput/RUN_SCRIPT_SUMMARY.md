# run.sh Script - Quick Reference

## 🚀 One-Command Setup

```bash
# First time: Build everything and run
./run.sh

# Just run (if already built)
./run.sh run

# Just build (without running)
./run.sh build
```

## 📋 All Commands

| Command | What it does |
|---------|-------------|
| `./run.sh` | Build everything and run (default) |
| `./run.sh build` | Build only (no run) |
| `./run.sh run` | Run only (must be built first) |
| `./run.sh clean` | Clean all build files |
| `./run.sh deps` | Check system dependencies |
| `./run.sh opencv` | Build OpenCV only |
| `./run.sh help` | Show this help |

## 🔧 What it Automates

1. **✅ Dependency Check** - Verifies all required libraries
2. **✅ OpenCV Build** - Downloads and builds OpenCV 4.8.0 locally
3. **✅ Program Build** - Compiles the LVGL VideoInput program
4. **✅ Program Run** - Launches the application

## 🎨 Features

- **🟢 Colored output** for easy reading
- **📊 Progress indicators** for each step
- **❌ Error handling** with specific solutions
- **⚡ Parallel building** using all CPU cores
- **🔄 Smart caching** (skips already built components)

## 🛠️ Troubleshooting

```bash
# Check what's missing
./run.sh deps

# Clean and start fresh
./run.sh clean && ./run.sh

# Rebuild OpenCV only
./run.sh opencv
```

## 📖 Full Documentation

For complete details, see: **[Source/RUN_SCRIPT_GUIDE.md](Source/RUN_SCRIPT_GUIDE.md)** 