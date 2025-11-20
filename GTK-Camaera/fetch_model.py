#!/usr/bin/env python3
"""
FaceNet ONNX Model Fetcher
Tries multiple sources and methods to download the model
"""

import os
import sys
import hashlib
from pathlib import Path

# Try to import urllib
try:
    import urllib.request
    import urllib.error
    HAS_URLLIB = True
except ImportError:
    HAS_URLLIB = False

PROJECT_DIR = Path(__file__).parent
MODELS_DIR = PROJECT_DIR / "models"
MODEL_FILE = MODELS_DIR / "facenet.onnx"

# Model sources to try
SOURCES = [
    {
        "url": "https://cdn-lfs.huggingface.co/repos/Xenova/onnx-models/26bfc3e8b4eb4db2f1ce688a5f40d5d6b6844ea0/facenet.onnx",
        "name": "Hugging Face CDN - FaceNet VGGFace2",
        "size_mb": 85,
    },
    {
        "url": "https://raw.githubusercontent.com/deepinsight/insightface/master/model_zoo/arcface_r100_v1.onnx",
        "name": "InsightFace - ArcFace (via raw GitHub)",
        "size_mb": 150,
    },
]

def download_with_urllib(url, output_file, source_name):
    """Download using urllib with progress"""
    if not HAS_URLLIB:
        return False, "urllib not available"
    
    print(f"\n  Downloading: {source_name}")
    print(f"  URL: {url}")
    
    try:
        class ProgressBar:
            def __init__(self, total=100):
                self.total = total
                self.current = 0
                self.last_percent = 0
            
            def __call__(self, block_num, block_size, total_size):
                if total_size > 0:
                    self.current = min(block_num * block_size, total_size)
                    percent = int(100 * self.current / total_size)
                    if percent != self.last_percent:
                        self.last_percent = percent
                        print(f'\r  Progress: {percent}%', end='', flush=True)
        
        urllib.request.urlretrieve(url, output_file, ProgressBar())
        print()  # Newline after progress
        
        size = os.path.getsize(output_file)
        if size > 1000000:  # > 1 MB
            return True, f"Downloaded {size / (1024*1024):.1f} MB"
        else:
            return False, f"File too small: {size} bytes"
    
    except Exception as e:
        return False, str(e)

def main():
    print("=" * 70)
    print("FaceNet ONNX Model Downloader")
    print("=" * 70)
    
    # Create models directory
    MODELS_DIR.mkdir(parents=True, exist_ok=True)
    
    # Check existing file
    if MODEL_FILE.exists():
        size = os.path.getsize(MODEL_FILE)
        if size > 1000000:  # > 1 MB
            print(f"\n✓ Valid model already exists: {size / (1024*1024):.1f} MB")
            return 0
        else:
            print(f"\n✗ Existing model is invalid ({size} bytes), removing...")
            MODEL_FILE.unlink()
    
    print(f"\nTarget: {MODEL_FILE}")
    print(f"Trying {len(SOURCES)} sources...\n")
    
    for i, source in enumerate(SOURCES, 1):
        print(f"\n[{i}/{len(SOURCES)}] {source['name']}")
        
        # Try download
        success, msg = download_with_urllib(source['url'], str(MODEL_FILE), source['name'])
        
        if success:
            print(f"✓ {msg}")
            print("\n" + "=" * 70)
            print("✅ SUCCESS! Model downloaded.")
            print("=" * 70)
            print(f"\nModel: {MODEL_FILE}")
            print(f"Size: {os.path.getsize(MODEL_FILE) / (1024*1024):.1f} MB")
            print("\nNext steps:")
            print("1. cd /home/shkwon/Projects/LVGL/GTK-Camaera")
            print("2. ./run.sh")
            return 0
        else:
            print(f"✗ Failed: {msg}")
            if MODEL_FILE.exists():
                MODEL_FILE.unlink()
    
    # All failed
    print("\n" + "=" * 70)
    print("❌ Could not download from any source")
    print("=" * 70)
    print("\nAlternative solutions:")
    print("\n1. MANUAL BROWSER DOWNLOAD (Recommended):")
    print("   - Open any of these URLs in your browser:")
    print("   - https://huggingface.co/Xenova/onnx-models/tree/main/models/facial-recognition")
    print("   - https://github.com/deepinsight/insightface/releases")
    print("   - Save the .onnx file to: ~/Downloads/facenet.onnx")
    print("   - Then run: cp ~/Downloads/facenet.onnx models/facenet.onnx")
    print("\n2. CHECK INTERNET CONNECTION:")
    print("   - ping 8.8.8.8")
    print("   - curl -I https://github.com")
    print("\n3. TRY WITH CURL DIRECTLY:")
    print("   cd models/")
    print("   curl -L -o facenet.onnx 'https://huggingface.co/Xenova/onnx-models/raw/main/models/facial-recognition/facenet.onnx'")
    print("\nFor help, read: DOWNLOAD_INSTRUCTIONS.md")
    return 1

if __name__ == "__main__":
    sys.exit(main())
