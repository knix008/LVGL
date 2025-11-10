#!/usr/bin/env python3
"""
Generate button icon images in multiple formats (PNG, JPG, BMP, GIF)
Creates play, ok/checkmark, pause, and stop icons
"""

from PIL import Image, ImageDraw
import os

# Create assets directory if it doesn't exist
os.makedirs('assets/icons', exist_ok=True)

# Icon size (will be used in 120x120 buttons, so make them 60x60 to have padding)
ICON_SIZE = 60
ICON_COLOR = (255, 255, 255, 255)  # White with full opacity

# Button background colors matching main.c styles
BUTTON_COLORS = {
    'play': (33, 150, 243),    # Blue #2196F3
    'ok': (76, 175, 80),        # Green #4CAF50
    'pause': (255, 152, 0),     # Orange #FF9800
    'stop': (244, 67, 54)       # Red #F44336
}

def create_play_icon(bg_color):
    """Create a play triangle icon"""
    img = Image.new('RGB', (ICON_SIZE, ICON_SIZE), bg_color)
    draw = ImageDraw.Draw(img)

    # Play triangle (pointing right)
    triangle = [
        (15, 8),    # Top left
        (15, 52),   # Bottom left
        (52, 30)    # Right point
    ]
    draw.polygon(triangle, fill=ICON_COLOR[:3])  # RGB only

    return img

def create_ok_icon(bg_color):
    """Create a checkmark/OK icon"""
    img = Image.new('RGB', (ICON_SIZE, ICON_SIZE), bg_color)
    draw = ImageDraw.Draw(img)

    # Checkmark
    # Short line (going down-right)
    draw.line([(12, 30), (23, 45)], fill=ICON_COLOR[:3], width=6)
    # Long line (going up-right)
    draw.line([(23, 45), (48, 12)], fill=ICON_COLOR[:3], width=6)

    return img

def create_pause_icon(bg_color):
    """Create a pause icon (two vertical bars)"""
    img = Image.new('RGB', (ICON_SIZE, ICON_SIZE), bg_color)
    draw = ImageDraw.Draw(img)

    # Left bar
    draw.rectangle([(15, 12), (24, 48)], fill=ICON_COLOR[:3])
    # Right bar
    draw.rectangle([(36, 12), (45, 48)], fill=ICON_COLOR[:3])

    return img

def create_stop_icon(bg_color):
    """Create a stop icon (square)"""
    img = Image.new('RGB', (ICON_SIZE, ICON_SIZE), bg_color)
    draw = ImageDraw.Draw(img)

    # Square
    draw.rectangle([(12, 12), (48, 48)], fill=ICON_COLOR[:3])

    return img

def save_in_all_formats(img, name):
    """Save image in PNG, JPG, BMP, and GIF formats"""
    # PNG
    img.save(f'assets/icons/{name}.png', 'PNG')

    # JPG
    img.save(f'assets/icons/{name}.jpg', 'JPEG', quality=95)

    # BMP
    img.save(f'assets/icons/{name}.bmp', 'BMP')

    # GIF - Match the format of the working ImageButton8.4 example
    # Convert to P mode (palette) and save with loop/duration like the reference GIF
    gif_img = img.convert('P', palette=Image.ADAPTIVE, colors=256)
    gif_img.save(f'assets/icons/{name}.gif', 'GIF', save_all=True, duration=500, loop=0)

    print(f"Created {name} icons: PNG, JPG, BMP, GIF")

# Create all icons
print("Generating button icons...")
print("-" * 50)

icons = {
    'play': create_play_icon(BUTTON_COLORS['play']),
    'ok': create_ok_icon(BUTTON_COLORS['ok']),
    'pause': create_pause_icon(BUTTON_COLORS['pause']),
    'stop': create_stop_icon(BUTTON_COLORS['stop'])
}

for name, img in icons.items():
    save_in_all_formats(img, name)

print("-" * 50)
print(f"All icons created successfully in assets/icons/")
print(f"Total files: {len(icons) * 4} (4 formats × {len(icons)} icons)")
print("Icon colors:")
print(f"  Play: Blue RGB{BUTTON_COLORS['play']}")
print(f"  OK: Green RGB{BUTTON_COLORS['ok']}")
print(f"  Pause: Orange RGB{BUTTON_COLORS['pause']}")
print(f"  Stop: Red RGB{BUTTON_COLORS['stop']}")
