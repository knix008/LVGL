#!/usr/bin/env python3
"""
Generate button icon images in multiple formats (PNG, JPG, BMP, GIF)
Creates play, ok/checkmark, pause, and stop icons
"""

from PIL import Image, ImageDraw
import os

# Create assets directory if it doesn't exist
os.makedirs('assets/icons', exist_ok=True)

# Icon size (will be used in 120x120 buttons, so make them 80x80 to have padding)
ICON_SIZE = 80
ICON_COLOR = (255, 255, 255, 255)  # White with full opacity
BG_COLOR = (0, 0, 0, 0)  # Transparent background

def create_play_icon():
    """Create a play triangle icon"""
    img = Image.new('RGBA', (ICON_SIZE, ICON_SIZE), BG_COLOR)
    draw = ImageDraw.Draw(img)

    # Play triangle (pointing right)
    triangle = [
        (20, 10),   # Top left
        (20, 70),   # Bottom left
        (70, 40)    # Right point
    ]
    draw.polygon(triangle, fill=ICON_COLOR)

    return img

def create_ok_icon():
    """Create a checkmark/OK icon"""
    img = Image.new('RGBA', (ICON_SIZE, ICON_SIZE), BG_COLOR)
    draw = ImageDraw.Draw(img)

    # Checkmark
    # Short line (going down-right)
    draw.line([(15, 40), (30, 60)], fill=ICON_COLOR, width=8)
    # Long line (going up-right)
    draw.line([(30, 60), (65, 15)], fill=ICON_COLOR, width=8)

    return img

def create_pause_icon():
    """Create a pause icon (two vertical bars)"""
    img = Image.new('RGBA', (ICON_SIZE, ICON_SIZE), BG_COLOR)
    draw = ImageDraw.Draw(img)

    # Left bar
    draw.rectangle([(20, 15), (32, 65)], fill=ICON_COLOR)
    # Right bar
    draw.rectangle([(48, 15), (60, 65)], fill=ICON_COLOR)

    return img

def create_stop_icon():
    """Create a stop icon (square)"""
    img = Image.new('RGBA', (ICON_SIZE, ICON_SIZE), BG_COLOR)
    draw = ImageDraw.Draw(img)

    # Square
    draw.rectangle([(15, 15), (65, 65)], fill=ICON_COLOR)

    return img

def save_in_all_formats(img, name):
    """Save image in PNG, JPG, BMP, and GIF formats"""
    # PNG (supports transparency)
    img.save(f'assets/icons/{name}.png', 'PNG')

    # JPG (no transparency, so add white background)
    jpg_img = Image.new('RGB', (ICON_SIZE, ICON_SIZE), (255, 255, 255))
    jpg_img.paste(img, (0, 0), img)
    jpg_img.save(f'assets/icons/{name}.jpg', 'JPEG', quality=95)

    # BMP (convert to RGB)
    bmp_img = Image.new('RGB', (ICON_SIZE, ICON_SIZE), (255, 255, 255))
    bmp_img.paste(img, (0, 0), img)
    bmp_img.save(f'assets/icons/{name}.bmp', 'BMP')

    # GIF (supports transparency)
    img.save(f'assets/icons/{name}.gif', 'GIF', transparency=0)

    print(f"Created {name} icons: PNG, JPG, BMP, GIF")

# Create all icons
print("Generating button icons...")
print("-" * 50)

icons = {
    'play': create_play_icon(),
    'ok': create_ok_icon(),
    'pause': create_pause_icon(),
    'stop': create_stop_icon()
}

for name, img in icons.items():
    save_in_all_formats(img, name)

print("-" * 50)
print(f"All icons created successfully in assets/icons/")
print(f"Total files: {len(icons) * 4} (4 formats × {len(icons)} icons)")
