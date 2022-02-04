#include "main_app.h"

//protoypes
void blinkLed(void);
static void led_task( void *pvParameters );
void main_app_init(void)
{
	  xTaskCreate( led_task, ( signed char * ) "LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY , NULL );
	  vTaskStartScheduler();
}
static void led_task( void *pvParameters )
{
    while(1)
    {
    	blinkLed();
    }
}

void blinkLed(void)
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	        HAL_Delay(1000);
}
