/** _write.c
 *
 * Billy Stevens
 *
 * Implements _write for stdio.h using framebuffer.
 * stdout in white, stderr in red, anything else in blue.
 * Only implements '\r' and '\n' other control codes are ignored.
 **/

#include "bsp/stm32746g_discovery_lcd.h"

uint32_t curPos = 0;

ssize_t handle_escape(const uint8_t *buf, size_t nbyte) {
    // Calculate screen dimensions.
    uint16_t fWidth = BSP_LCD_GetFont()->Width;
    uint16_t fHeight = BSP_LCD_GetFont()->Height;

    uint32_t sWidth = BSP_LCD_GetXSize() / fWidth;
    uint32_t sHeight = BSP_LCD_GetYSize() / fHeight;

    size_t curLine = curPos / sWidth;
    size_t curCol = curPos % sWidth;

    size_t i = 0;
    for (; i < nbyte; i++) {
        char c = buf[i];
        // Convert lowercase to uppercase.
        if (c >= 'a' && c <= 'z') {
            c -= 0x20;
        }

        // J clears lines from screen
        if (c == 'J') {
            // J or 0J clears screen below cursor.
            if (i == 0 || (i == 1 && buf[i - 1] == '0')) {
                for (size_t l = curLine; l < sHeight; l++) {
                    BSP_LCD_ClearStringLine(l);
                }
            }
            // 1J clears screen above cursor.
            else if (i == 1 && buf[i - 1] == '1') {
                for (size_t l = 0; l <= curLine; l++) {
                    BSP_LCD_ClearStringLine(l);
                }
            }
            // 1J clears entire screen.
            else if (i == 1 && buf[i - 1] == '2') {
                for (size_t l = 0; l <= sHeight; l++) {
                    BSP_LCD_ClearStringLine(l);
                }
            }
        }

        // K clears characters from line
        if (c == 'K') {
            size_t curLine = curPos / sWidth;
            // K or 0K clears line after cursor.
            if (i == 0 || (i == 1 && buf[i - 1] == '0')) {
                for (size_t n = curCol; n < sWidth; n++) {
                    BSP_LCD_DisplayChar(n * fWidth, curLine * fHeight, ' ');
                }
            }
            // 1K clears line before cursor.
            else if (i == 1 && buf[i - 1] == '1') {
                for (size_t n = 0; n <= curCol; n++) {
                    BSP_LCD_DisplayChar(n * fWidth, curLine * fHeight, ' ');
                }
            }
            // 1K clears entire line
            else if (i == 1 && buf[i - 1] == '2') {
                BSP_LCD_ClearStringLine(curLine);
            }
        }

        // Letter characters end escape sequence.
        if (c >= 'A' && c <= 'Z') {
            break;
        }
        // Else, must be part of escape sequence keep reading.
    }
    return i + 1; // Return number of characters processed.
}

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
        size_t curLine = curPos / sWidth;
        size_t curCol = curPos % sWidth;

        // Handle printable char
        if (buf[i] >= 0x20 && buf[i] < 0x7F) {
            BSP_LCD_DisplayChar(curCol * fWidth, curLine * fHeight, buf[i]);
            curPos++;
        }
        // Handle NL
        else if (buf[i] == '\n') {
            BSP_LCD_ClearStringLine(curLine + 1); // Clear next line.
            curPos += sWidth - curCol;            // Advance to start of next line.
        }
        // Handle CR
        else if (buf[i] == '\r') {
            curPos -= curCol; // Return to start of line.
        }
        // Handle escape
        else if (buf[i] == 0x1B) {
            if (i + 1 < nbyte) {
                switch (buf[i + 1]) {
                case 'M':
                    // Handle reverse newline.
                    curPos -= sWidth + curCol; // Return to start of previous line.
                    i++;                       // Advance past 'M'
                    break;
                case '[':
                    i++; // Advance past '['
                    // Handle multicharacter escape.
                    if (i + 1 < nbyte) {
                        i += handle_escape(&buf[i + 1], nbyte - (i + 1));
                    }
                    break;
                }
            }
        }
    }
    return i;
}
