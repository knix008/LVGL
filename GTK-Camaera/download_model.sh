#!/bin/bash

# Download FaceNet ONNX Model Script
# Downloads a pre-trained FaceNet model in ONNX format
# Supports multiple download methods: curl, wget, or Python

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODEL_DIR="$SCRIPT_DIR/models"
MODEL_FILE="$MODEL_DIR/facenet.onnx"

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}======================================"
echo "FaceNet ONNX Model Downloader"
echo "======================================${NC}"
echo ""

# Create models directory if it doesn't exist
if [ ! -d "$MODEL_DIR" ]; then
    mkdir -p "$MODEL_DIR"
    echo -e "${GREEN}✓ Created models directory${NC}"
fi

# Check if model already exists
if [ -f "$MODEL_FILE" ] && [ -s "$MODEL_FILE" ]; then
    SIZE=$(du -h "$MODEL_FILE" | cut -f1)
    echo -e "${GREEN}✓ FaceNet model already exists (size: $SIZE)${NC}"
    exit 0
fi

echo "Downloading FaceNet ONNX model..."
echo ""
echo "This model is from the DeepFace project:"
echo "  - Pre-trained on VGGFace2 dataset"
echo "  - FaceNet-128D embeddings (128-dimensional face vectors)"
echo "  - Suitable for face recognition tasks"
echo ""

# Try multiple sources for the model
MODEL_URLS=(
    "https://huggingface.co/doguscank/facenet-onnx/resolve/main/facenet.onnx"
    "https://huggingface.co/doguscank/facenet-onnx/raw/main/facenet.onnx"
)

DOWNLOAD_SUCCESS=0

for url in "${MODEL_URLS[@]}"; do
    echo "Trying: $url"

    # Try curl first
    if command -v curl &> /dev/null; then
        if curl -L --max-time 60 -o "$MODEL_FILE" "$url" 2>/dev/null; then
            if [ -s "$MODEL_FILE" ] && [ $(stat -f%z "$MODEL_FILE" 2>/dev/null || stat -c%s "$MODEL_FILE" 2>/dev/null) -gt 1000000 ]; then
                echo -e "${GREEN}✓ Downloaded successfully${NC}"
                DOWNLOAD_SUCCESS=1
                break
            else
                rm -f "$MODEL_FILE"
            fi
        else
            rm -f "$MODEL_FILE"
        fi
    # Fallback to wget
    elif command -v wget &> /dev/null; then
        if wget --timeout=60 -O "$MODEL_FILE" "$url" 2>/dev/null; then
            if [ -s "$MODEL_FILE" ] && [ $(stat -f%z "$MODEL_FILE" 2>/dev/null || stat -c%s "$MODEL_FILE" 2>/dev/null) -gt 1000000 ]; then
                echo -e "${GREEN}✓ Downloaded successfully${NC}"
                DOWNLOAD_SUCCESS=1
                break
            else
                rm -f "$MODEL_FILE"
            fi
        else
            rm -f "$MODEL_FILE"
        fi
    fi
done

if [ $DOWNLOAD_SUCCESS -eq 0 ]; then
    echo -e "${RED}✗ Download failed${NC}"
    echo ""
    echo -e "${YELLOW}SOLUTIONS:${NC}"
    echo ""
    echo "1. CHECK NETWORK:"
    echo "   - ping 8.8.8.8"
    echo "   - curl -I https://github.com (should respond quickly)"
    echo ""
    echo "2. DOWNLOAD ON ANOTHER MACHINE & COPY:"
    echo "   - Download from: https://huggingface.co/doguscank/facenet-onnx/tree/main"
    echo "   - Save facenet.onnx to models/ directory"
    echo ""
    echo "3. USE SCP/USB TRANSFER:"
    echo "   - Transfer the .onnx file from another machine to:"
    echo "   - $MODEL_FILE"
    echo ""
    echo "4. CREATE STUB MODEL FOR TESTING:"
    echo "   - touch $MODEL_FILE"
    echo ""
    exit 1
fi

# Verify the downloaded file
if [ -f "$MODEL_FILE" ]; then
    SIZE=$(du -h "$MODEL_FILE" | cut -f1)
    echo ""
    echo -e "${GREEN}✓ Model downloaded successfully (size: $SIZE)${NC}"
    echo "  Location: $MODEL_FILE"
    echo ""
    echo -e "${GREEN}Ready to use! Run: ./run.sh${NC}"
fi
