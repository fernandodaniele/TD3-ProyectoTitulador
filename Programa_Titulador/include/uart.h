#ifndef UART_H
#define UART_H

/*==================[Inclusiones]============================================*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"
#include "driver/uart.h"

/*==================[Definiciones]===================================*/

#define UART_NUM            UART_NUM_2
#define BUFFER_SIZE         1024
#define TASK_MEMORY         1024 * 2
#define TX                  GPIO_NUM_4
#define RX                  GPIO_NUM_5

uart_config_t uart_config = {

    .baud_rate  = 115200,
    .data_bits  = UART_DATA_8_BITS, // ---UART_DATA_8_BITS---
    .parity     = UART_PARITY_DISABLE,
    .stop_bits  = UART_STOP_BITS_1,
    .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB

};

typedef struct
{
    bool Habilitador_Limpieza;
    bool Giro_Limpieza;
    char Volumen_Inyeccion_str[4];
    int Volumen_Inyeccion;
}Limpieza;

/*==================[Prototipos de funciones]======================*/

void init_uart();
void fin_titulacion();
void volumen_suma_1();
void volumen_suma_01();
void eliminar_volumen_registrado();
void registrar_volumen_inflexion(float ptr);

#endif