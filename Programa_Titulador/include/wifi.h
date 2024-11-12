#ifndef WIFI_H
#define WIFI_H

/*==================[Inclusiones]============================================*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"

/*==================[Definiciones]===================================*/

#define WIFI_SSID   "."
#define WIFI_PASS   "43364336"

/*==================[Prototipos de funciones]======================*/

void wifi_init();

#endif