/** _write.c
 *
 *  Billy Stevens
 *
 *  Implements _write for stdio.h using framebuffer.
 * stdout in white, stderr in red, anything else in blue.
 * Only implements '\r' and '\n' other control codes are ignored.
 **/

#include <stdio.h>
#include <stdlib.h>

#include "bsp/stm32746g_discovery_lcd.h"
#include "bsp/stm32746g_discovery_ts.h"
#include "stm32f7xx_hal.h"

int main() {
    // Initialize LED GPIO.
    BSP_LED_Init(LED1);

    // Initialize LCD and framebuffer.
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayOn();

    // Initialize touch screen.
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_OK) {
        fprintf(stderr, "Touchscreen connected.\n");
    } else {
        fprintf(stderr, "Touchscreen not responding.\n");
    }

    TS_StateTypeDef ts_state;
    while (1) {
        if (BSP_TS_GetState(&ts_state) != TS_OK) {
            fprintf(stderr, "Touchscreen read failed.\n");
        }

        if(ts_state.touchDetected != 0) {
            fprintf(stdout, "Position %d, %d.\x1B[K\n\x1BM", ts_state.touchX[0], ts_state.touchY[0]);
            fflush(stdout);
        }
    }
    // Infinite loop, never return.
}
