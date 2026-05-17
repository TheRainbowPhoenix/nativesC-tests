#include "nrender.hpp"
#include <os/lcd.h>
namespace nrender {
#define FONT_ADDRESS_1 0x8c1a70cc
const PegFont* pSystemFont1 = (const PegFont*)FONT_ADDRESS_1;
void fill_rect(int x1, int y1, int x2, int y2, uint16_t color) {
    uint16_t* vram = LCD_GetVRAMAddress();
    unsigned int sw, sh; LCD_GetSize(&sw, &sh);
    if (x1 < 0) x1 = 0; if (y1 < 0) y1 = 0; if (x2 > (int)sw) x2 = (int)sw; if (y2 > (int)sh) y2 = (int)sh;
    for (int y = y1; y < y2; y++) {
        uint16_t* row = vram + y * sw;
        for (int x = x1; x < x2; x++) row[x] = color;
    }
}
int get_char_width(TCHAR ch, const PegFont* pFont) {
    if (!pFont || ch < pFont->wFirstChar || ch >= pFont->wLastChar) return 0;
    int charIndex = ch - pFont->wFirstChar;
    return (int)(pFont->pOffsets[charIndex + 1] - pFont->pOffsets[charIndex]);
}
int get_text_width(const char* text, const PegFont* pFont) {
    if (!text) return 0;
    int w = 0;
    while (*text) w += get_char_width(*text++, pFont);
    return w;
}
void draw_char(int x, int y, TCHAR ch, uint16_t color, const PegFont* pFont) {
    int width = get_char_width(ch, pFont);
    if (width <= 0) return;
    uint16_t* framebuffer = LCD_GetVRAMAddress();
    unsigned int buffer_width, buffer_height; LCD_GetSize(&buffer_width, &buffer_height);
    int startByteOffset = (int)pFont->pOffsets[ch - pFont->wFirstChar];
    for (int cy = 0; cy < (int)pFont->uHeight; cy++) {
        for (int cx = 0; cx < width; cx++) {
            int bitPos = (startByteOffset * 8) + (cy * (int)pFont->wBytesPerLine * 8) + cx;
            int byteIndex = bitPos / 8;
            int bitInByte = 7 - (bitPos % 8);
            if ((pFont->pData[byteIndex] >> bitInByte) & 1) {
                int screenX = x + cx; int screenY = y + cy;
                if (screenX >= 0 && (unsigned int)screenX < buffer_width && screenY >= 0 && (unsigned int)screenY < buffer_height) {
                    framebuffer[screenY * buffer_width + screenX] = color;
                }
            }
        }
    }
}
void draw_text(int x, int y, const char* text, uint16_t color, const PegFont* pFont) {
    if (!text) return;
    while (*text) {
        draw_char(x, y, *text, color, pFont);
        x += get_char_width(*text, pFont);
        text++;
    }
}
} // namespace nrender
