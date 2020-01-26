// -u---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include "bsp/stm32746g_discovery_lcd.h"

// ----- main() ---------------------------------------------------------------
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main() {
	BSP_LED_Init(LED1);

	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayOn();

	puts("Standard output message.");
	fprintf(stderr, "Standard error message.\n");

	unsigned int cycles = 0;
	while (1) {
		BSP_LED_Toggle(LED1);
		for(size_t i ; i < 80000000; i++);

		++cycles;
		printf("Seconds %d.\r", cycles);
	}
	// Infinite loop, never return.
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
