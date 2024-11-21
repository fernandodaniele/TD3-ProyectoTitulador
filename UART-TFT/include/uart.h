/*=============================================================================
 * Autor: Fernando Ezequiel Daniele <fernandodaniele1993@gmai.com>
 * Fecha: 2020/12/21
 * Versión 2.0, fecha: 2024/11/15
 *===========================================================================*/
#ifndef UART_H
#define UART_H 
#include "Arduino.h"

/**
 * @brief Inicia la UART en 115200 baudios
 * 
 */
void iniciarUart ();

/**
 * @brief Espera a recibir comando J para saber si finalizó la titulación
 * 
 * @return byte: 1 finalizó titulación, 0 continua titulación 
 */
byte estadoTitulacion(float *val);

/**
 * @brief Envia comando A
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte iniciarCalibracion();

/**
 * @brief Envia comando B
 * 
 * @return byte: 1 OK, 0 ERROR  
 */
byte finalizarCalibracion();

/**
 * @brief Envia comando C
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte leerPotencial(float *val);

/**
 * @brief Envia comando D
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte calibrarBufferA();

/**
 * @brief Envia comando E
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte calibrarBufferB();

/**
 * @brief Envia comando F
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte calibrarBufferC();

/**
 * @brief Envia comando G
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte iniciarTitulacion();

/**
 * @brief Envia comando I
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte cancelarTitulacion();

/**
 * @brief Envia comando K
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte iniciarLimpieza();

/**
 * @brief Envia comando L
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte finalizarLimpieza();

/**
 * @brief Envia comando M
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte leerVolumenCorte(int *val);

/**
 * @brief Envia comando N
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte guardarVolumenCorte(char * val);

/**
 * @brief Envia comando O
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte InyectarVolumen(char * val);

/**
 * @brief Envia comando P
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte habilitarAgitador();

/**
 * @brief Envia comando Q
 * 
 * @return byte: 1 OK, 0 ERROR 
 */
byte deshabilitarAgitador();

#endif