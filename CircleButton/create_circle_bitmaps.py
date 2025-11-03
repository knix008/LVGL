#!/usr/bin/env python3
"""
Create circular bitmap images for LVGL circle buttons
"""

from PIL import Image, ImageDraw

def create_circle_bitmap(filename, size, fill_color, border_color=None, border_width=0):
    """
    Create a circular bitmap image with transparency

    Args:
        filename: Output BMP filename (actually PNG for transparency)
        size: Diameter of the circle (width and height)
        fill_color: RGB tuple for fill color
        border_color: RGB tuple for border (optional)
        border_width: Width of border in pixels
    """
    # Create image with transparency (RGBA mode)
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Draw filled circle
    if border_width > 0 and border_color:
        # Draw border circle
        draw.ellipse([0, 0, size-1, size-1], fill=border_color, outline=border_color)
        # Draw inner filled circle
        margin = border_width
        draw.ellipse([margin, margin, size-1-margin, size-1-margin],
                     fill=fill_color, outline=fill_color)
    else:
        # Draw simple filled circle
        draw.ellipse([0, 0, size-1, size-1], fill=fill_color, outline=fill_color)

    # Create a circular mask for perfect transparency outside the circle
    mask = Image.new('L', (size, size), 0)
    mask_draw = ImageDraw.Draw(mask)
    mask_draw.ellipse([0, 0, size-1, size-1], fill=255)

    # Apply mask to make corners truly transparent
    img.putalpha(mask)

    # Save both PNG and BMP versions
    # PNG with transparency
    png_filename = filename.replace('.bmp', '.png')
    img.save(png_filename, 'PNG')
    print(f"Created: {png_filename}")

    # BMP with light gray background matching the screen (0xF5F5F5)
    img_bmp = Image.new('RGB', (size, size), (245, 245, 245))
    img_bmp.paste(img, (0, 0), img)
    img_bmp.save(filename, 'BMP')
    print(f"Created: {filename}")

def main():
    # Create circular bitmaps for each button
    size_normal = 60  # Circle diameter for normal state
    size_pressed = 63  # Circle diameter for pressed state (5% larger)

    # Circle 1: Red/Pink - Normal state
    create_circle_bitmap(
        'assets/images/circle_red.bmp',
        size_normal,
        (233, 30, 99),  # 0xE91E63
        (51, 51, 51),   # 0x333333 border
        2
    )

    # Circle 1: Red/Pink - Pressed state
    create_circle_bitmap(
        'assets/images/circle_red_pressed.bmp',
        size_pressed,
        (233, 30, 99),  # 0xE91E63
        (51, 51, 51),   # 0x333333 border
        2
    )

    # Circle 2: Green - Normal state
    create_circle_bitmap(
        'assets/images/circle_green.bmp',
        size_normal,
        (76, 175, 80),  # 0x4CAF50
        (51, 51, 51),   # 0x333333 border
        2
    )

    # Circle 2: Green - Pressed state
    create_circle_bitmap(
        'assets/images/circle_green_pressed.bmp',
        size_pressed,
        (76, 175, 80),  # 0x4CAF50
        (51, 51, 51),   # 0x333333 border
        2
    )

    # Circle 3: Blue - Normal state
    create_circle_bitmap(
        'assets/images/circle_blue.bmp',
        size_normal,
        (33, 150, 243),  # 0x2196F3
        (51, 51, 51),    # 0x333333 border
        2
    )

    # Circle 3: Blue - Pressed state
    create_circle_bitmap(
        'assets/images/circle_blue_pressed.bmp',
        size_pressed,
        (33, 150, 243),  # 0x2196F3
        (51, 51, 51),    # 0x333333 border
        2
    )

    print("\nCircular images created successfully!")
    print("Files created in assets/images/:")
    print("  PNG (with transparency):")
    print("    - circle_red.png")
    print("    - circle_green.png")
    print("    - circle_blue.png")
    print("  BMP (with light gray background matching screen):")
    print("    - circle_red.bmp")
    print("    - circle_green.bmp")
    print("    - circle_blue.bmp")

if __name__ == '__main__':
    main()
