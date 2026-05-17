#pragma once
#include <stdint.h>
namespace nrender {
typedef uint16_t WORD;
typedef uint8_t  UCHAR;
typedef char     TCHAR;
typedef struct PegFont_t {
  UCHAR uType;
  UCHAR uAscent;
  UCHAR uDescent;
  UCHAR uHeight;
  WORD  wBytesPerLine;
  WORD  wFirstChar;
  WORD  wLastChar;
  WORD *pOffsets;
  void *pNext;
  UCHAR *pData;
} PegFont;
extern const PegFont* pSystemFont1;
void fill_rect(int x1, int y1, int x2, int y2, uint16_t color);
int get_char_width(TCHAR ch, const PegFont* pFont);
int get_text_width(const char* text, const PegFont* pFont);
void draw_char(int x, int y, TCHAR ch, uint16_t color, const PegFont* pFont);
void draw_text(int x, int y, const char* text, uint16_t color, const PegFont* pFont);
} // namespace nrender
