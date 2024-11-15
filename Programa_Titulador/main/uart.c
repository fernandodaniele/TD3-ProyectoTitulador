/*==================[ Inclusiones ]============================================*/
#include "../include/uart.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "esp_log.h"

/*==================[ Definiciones ]===================================*/

#define PROCESADORA         0
#define PROCESADORB         1
#define T_GUARDADO          10     // Timepo de espera entre datos leidos

/*==================[Prototipos de funciones]======================*/

esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config);
esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);
esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t* uart_queue, int intr_alloc_flags);

void TaskUart(void *taskParmPtr);      // Prototipo de la función de la tarea

/*==================[Variables]==============================*/

static const char *TAG_UART = "UART";

extern gpio_int_type_t P_Enable_Bomba;

extern float Vout_PH;
char valor[6];

bool flag_Agitador  = false;
bool flag_Titular   = false;
//bool flag_Limpieza = false;
//uint8_t flag_Calibracion = 0;

int Volumen_Comp = 35;      // Se coloca como valor de volumen de comp
char Volumen_Anterior[4] = "35";
char Volumen_Inflexion_Muestreo[6];

Limpieza limpieza;

// Variables referentes al Volumen
float Volumen_Inflexion         = 0.0;
float dif_guardado              = 0.0;
float volumen_registrado        = 0.0;
float volumen_registrado_ant    = 0.0;

//char *msg = "OK\r\n";
char *msg = "K";

const char *Calibrar_ON         = "A";
const char *Calibrar_OFF        = "B";
const char *Lectura_ADC         = "C";
const char *Buffer_4            = "D";
const char *Buffer_7            = "E";
const char *Buffer_10           = "F";
const char *Titular_ON          = "G";
const char *Lectura_PH          = "H";
const char *Titular_OFF         = "I";
const char *Titular_END         = "J";  
const char *Limpieza_ON         = "K";
const char *Limpieza_OFF        = "L";
const char *Volumen             = "M";
const char *Guardar_Volumen     = "N";
const char *Estado_Agitador     = "O";
const char *Agitador_ON         = "P";
const char *Agitador_OFF        = "Q";
const char *Volumen_Inyeccion   = "R";

// int largo, largo2;

extern QueueHandle_t S_Agitador;
//extern SemaphoreHandle_t S_Limpieza;
extern QueueHandle_t S_Limpieza;
extern QueueHandle_t S_Calibracion;
extern QueueHandle_t S_Titulacion;
extern QueueHandle_t S_Inyeccion;

/*==================[Implementaciones]=================================*/

/**
 * @brief Inicializacion de la UART
 *
 */
void init_uart()
{
    uart_param_config(UART_NUM, &uart_config);

    uart_set_pin(UART_NUM, TX, RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uart_driver_install(UART_NUM, BUFFER_SIZE, BUFFER_SIZE, 0, NULL, 0);

    BaseType_t err = xTaskCreatePinnedToCore(
        TaskUart,                     	// Funcion de la tarea a ejecutar
        "TaskUart",   	                // Nombre de la tarea como String amigable para el usuario
        TASK_MEMORY, 		            // Cantidad de stack de la tarea
        NULL,                          	// Parametros de tarea
        tskIDLE_PRIORITY+1,         	// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
        NULL,                           // Puntero a la tarea creada en el sistema
        PROCESADORA                     // Numero de procesador
    );

    if(err == pdFAIL)
    {
        ESP_LOGI(TAG_UART, "Error al crear la tarea.");
        while(1);    // Si no pudo crear la tarea queda en un bucle infinito
    }

    ESP_LOGI(TAG_UART, "Inicialización de la UART completa");

}

/**
 * @brief Lectura del Puerto Serie
 *
 * @param taskParmPtr
 */
void TaskUart(void *taskParmPtr)
{
    /*==================[Configuraciones]======================*/

    char *Dato = (char*) malloc(BUFFER_SIZE);     // Puntero toma el dato leido, con "malloc" se le asigna un espacio de memoria

    /*==================[Bucle]======================*/
    while(1)
    {
        bzero(Dato, BUFFER_SIZE); // Se borra el espacio de memoria que este en "Dato"

        int len = uart_read_bytes(UART_NUM, Dato, BUFFER_SIZE, pdMS_TO_TICKS(T_GUARDADO)); // Ver de poner el tiempo de lectura como portMAX_DELAY y sacar el continue 
        //int len = uart_read_bytes(UART_NUM, Dato, BUFFER_SIZE, portMAX_DELAY); 

        if(len == 0)
        {
            continue;
        }

        if(len > 1)
        {
            if(Dato[0] == 'N')
            {
                for(int i = 0; i < len - 1; i++)
                {
                    Volumen_Anterior[i] = Dato[i+1];
                } 
            }  
            
            if(Dato[0] == 'R')
            {
                for(int i = 0; i < len - 1; i++)
                {
                    limpieza.Volumen_Inyeccion_str[i] = Dato[i+1];
                } 
            }
        }

        ESP_LOGI(TAG_UART, "len -> %d", len);

        // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
        //uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
        ESP_LOGI(TAG_UART, "Dato recibido -> %s\n", Dato);
        

        // Recortamos el dato que se recibe
        Dato[1] = '\0';

        //ESP_LOGI(TAG_UART, "Largo Dato -> %d - %d", strlen(Dato), strlen(AI));
        ESP_LOGI(TAG_UART, "Dato recibido -> %s\n", Dato);
        ESP_LOGI(TAG_UART, "Dato recibido -> %s\n", Volumen_Anterior);

        // Se usan else if para dar prioridad a las comparaciones 
        if(strcmp(Dato, Lectura_ADC) == 0)
        {
            // ---Le enviamos el valor actual de PH leido al ATMega cuando se recibe "C"---
            // ---Además, le enviamos "/" justo despues del valor---
            snprintf(valor, sizeof(valor), "%.02f", Vout_PH);
            uart_write_bytes(UART_NUM, valor, sizeof(valor));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
        }

        else if(strcmp(Dato, Titular_ON) == 0)
        {
            // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
            gpio_set_level(P_Enable_Bomba, 0);
            flag_Titular = true;
        }

        else if(strcmp(Dato, Titular_OFF) == 0)
        {
            flag_Titular = false;
            gpio_set_level(P_Enable_Bomba, 1);
            //snprintf(Volumen_Inflexion_ptr, strlen(Volumen_Inflexion_ptr), "%.2f", Volumen_Inflexion);
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));

            fin_titulacion();
            eliminar_volumen_registrado();

            vTaskDelay(pdMS_TO_TICKS(200));
        }

        else if(strcmp(Dato, Agitador_ON) == 0)
        {
            //ESP_LOGI(TAG_UART, "Entrada a AI\n");
            // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
            flag_Agitador = true;
            xQueueSend(S_Agitador, &flag_Agitador, portMAX_DELAY);
        }

        else if(strcmp(Dato, Agitador_OFF) == 0)
        {
            // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
            flag_Agitador = false;
            xQueueSend(S_Agitador, &flag_Agitador, portMAX_DELAY);
        }

        else if(strcmp(Dato, Limpieza_ON) == 0)
        {
            // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
            limpieza.Giro_Limpieza = 0;
            limpieza.Habilitador_Limpieza = true;
            xQueueSend(S_Limpieza, &limpieza, portMAX_DELAY);
        }

        else if(strcmp(Dato, Limpieza_OFF) == 0)
        {
            // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
            limpieza.Habilitador_Limpieza = false;
            xQueueSend(S_Limpieza, &limpieza, portMAX_DELAY);
        }

        else if(strcmp(Dato, Buffer_4) == 0)
        {
            // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
            xQueueSend(S_Calibracion, &Buffer_4, portMAX_DELAY);
        }

        else if(strcmp(Dato, Buffer_7) == 0)
        {
            // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
            xQueueSend(S_Calibracion, &Buffer_7, portMAX_DELAY);
        }

        else if(strcmp(Dato, Buffer_10) == 0)
        {
            // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
            xQueueSend(S_Calibracion, &Buffer_10, portMAX_DELAY);
        }

        else if(strcmp(Dato, Volumen) == 0)
        {
            //snprintf(Volumen_Anterior, sizeof(Volumen_Anterior), "%d", Volumen_Guardado);
            uart_write_bytes(UART_NUM, Volumen_Anterior, sizeof(Volumen_Anterior));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
        }

        else if(strcmp(Dato, Guardar_Volumen) == 0)
        {
            // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
            
            if(len == 2)
            {
                Volumen_Anterior[len-1] = '\0';
            }
            else if(len == 3)
            {
                Volumen_Anterior[len-1] = '\0';
            }
            else if(len == 4)
            {
                Volumen_Anterior[len-1] = '\0';
            }
            
            //Volumen_Anterior[CARACTERES] = '\0'; 
            ESP_LOGI(TAG_UART, "Valor de Volumen Guardado -> %s", Volumen_Anterior);
            Volumen_Comp = atoi(Volumen_Anterior);
        }

        else if(strcmp(Dato, Volumen_Inyeccion) == 0)
        {
            // ---Le enviamos "OK" al ATMega cuando se recibe el dato---
            uart_write_bytes(UART_NUM, (const char*)msg, sizeof(msg));
            uart_write_bytes(UART_NUM, "/", sizeof(char));
            
            if(len == 2)
            {
                limpieza.Volumen_Inyeccion_str[len-1] = '\0';
            }
            else if(len == 3)
            {
                limpieza.Volumen_Inyeccion_str[len-1] = '\0';
            }
            else if(len == 4)
            {
                limpieza.Volumen_Inyeccion_str[len-1] = '\0';
            }
            
            //Volumen_Anterior[CARACTERES] = '\0'; 
            ESP_LOGI(TAG_UART, "Valor de Volumen de Inyeccion -> %s", limpieza.Volumen_Inyeccion_str);
            limpieza.Volumen_Inyeccion = atoi(limpieza.Volumen_Inyeccion_str);
            xQueueSend(S_Inyeccion, &limpieza, portMAX_DELAY);
        }

        else{ESP_LOGI(TAG_UART, "Dato Recibido Incorrecto");}
    }
}

void fin_titulacion()
{
    flag_Titular = false;
    gpio_set_level(P_Enable_Bomba, 1);
    snprintf(Volumen_Inflexion_Muestreo, sizeof(Volumen_Inflexion_Muestreo), "%.02f", Volumen_Inflexion);
    uart_write_bytes(UART_NUM, Titular_END, strlen(Titular_END));
    uart_write_bytes(UART_NUM, "/", 1);
    uart_write_bytes(UART_NUM, Volumen_Inflexion_Muestreo, sizeof(Volumen_Inflexion_Muestreo));
    uart_write_bytes(UART_NUM, "/", 1);
} 

void volumen_suma_1()
{
    ESP_LOGI(TAG_UART, "Suma 1ml");
    volumen_registrado_ant = volumen_registrado;
    volumen_registrado += 1.0;
}

void volumen_suma_01()
{
    volumen_registrado_ant = volumen_registrado;

    volumen_registrado += 0.1;
}

void eliminar_volumen_registrado()
{
    volumen_registrado  = 0.0;
    dif_guardado        = 0.0;
    Volumen_Inflexion   = 0.0;
    ESP_LOGI(TAG_UART, "Volumen -> %.03f", volumen_registrado);
}

void registrar_volumen_inflexion(float *ptr)
{
    dif_guardado = *ptr;
    Volumen_Inflexion = volumen_registrado;
}