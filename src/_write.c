/** _write.c
 *
 *  Billy Stevens
 *
 *  Implements _write for stdio.h using framebuffer.
 * stdout in white, stderr in red, anything else in blue.
 * Only implements '\r' and '\n' other control codes are ignored.
 **/

#include "bsp/stm32746g_discovery_lcd.h"

uint32_t cPos = 0;

ssize_t _write(int fd, const uint8_t *buf, size_t nbyte) {
    // Determine color based on file descriptior.
    if (fd == 1) {
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    } else if (fd == 2) {
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
    } else {
        BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    }

    // Calculate font size and screen limits.
    uint16_t fWidth = BSP_LCD_GetFont()->Width;
    uint16_t fHeight = BSP_LCD_GetFont()->Height;

    uint32_t sWidth = BSP_LCD_GetXSize() / fWidth;
    // uint32_t sHeight = BSP_LCD_GetYSize() / fHeight;

    // Print text buffer.
    size_t i = 0;
    for (; i < nbyte; i++) {
        if (buf[i] >= 0x20 && buf[i] < 0x7F) {
            // Handle printable char
            BSP_LCD_DisplayChar((cPos % sWidth) * fWidth, (cPos / sWidth) * fHeight, buf[i]);
            cPos++;
        } else if (buf[i] == '\n') {
            // Handle NL
            cPos = cPos + sWidth - (cPos % sWidth);
            BSP_LCD_ClearStringLine(cPos / sWidth);
        } else if (buf[i] == '\r') {
            // Handle CR
            cPos = cPos - (cPos % sWidth);
        }
    }
    return i;
}
