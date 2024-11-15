/*=============================================================================
 * Autor: Fernando Ezequiel Daniele <fernandodaniele1993@gmai.com>
 * Fecha: 2020/12/21
 *===========================================================================*/
#ifndef PANTALLA_H
#define PANTALLA_H
 /*=============================================================================
 * Declaración de funciones globales públicas
 *===========================================================================*/
 /**
  * @brief Inicializa el display TFT
  */
void inicializarTFT();

/**
 * @brief Muestra la pantalla de inicio
 * 
 */
void pantallaInicial();

/**
 * @brief Consulta si se presionó algún botón en la pantalla inicial
 * 
 * @return int 
 *  @li @c 0 No se presionó ningún boton
 *  @li @c 1 Botón superior izquierdo
 *  @li @c 2 Botón superior derecho
 *  @li @c 3 Botón inferior izquierdo
 *  @li @c 4 Botón inferior derecho
 */
int consultaTactil();

/**
 * @brief Muestra la pantalla con el valor actual de cada buffer y da
 *        la opción de elegir cada uno de ellos por separado para
 *        realizar la calibración
 */
void pantallaElegirBuffer();

/**
 * @brief Muestra la pantalla con el valor actual de ph que será guardado como 
 *        valor para calibrar el instrumento
 */
void pantallaCalibrar ();

/**
 * @brief Consulta si se presionó algún botón en la pantalla de elección de buffer
 * 
 * @return int 
 *  @li @c 0 No se presionó ningún boton
 *  @li @c 3 Botón de guardar
 *  @li @c 4 Botón de cancelar
 */
int tactilCalibrar();

/**
 * @brief Muestra la pantalla con la curva de titulación y el valor actual de pH
 * 
 */
void pantallaMedir();

/**
 * @brief Actualiza el grafico y consulta si se presionó el boton de finalizar
 * 
 * @return int 
 *  @li @c 0 No se presionó ningún boton
 *  @li @c 4 Botón de finalizar
 */
int tactilMedir();

/**
 * @brief Muestra la pantalla con la opciones para configurar los buffers a utilizar,
 * el volumen de corte y para ejecutar la limpieza de la bomba
 * 
 */
void pantallaAjustes();

/**
 * @brief Muestra la pantalla que permite ajustar el valor del volumen de corte
 * 
 */
void pantallaVolumenCorte();

/**
 * @brief Permite configurar el volumen de corte y consulta si se presionó el boton de guardar
 * 
 * @return int 
 *  @li @c 0 No se presionó ningún boton
 *  @li @c 4 Botón de finalizar
 */
int  tactilVolumenCorte();

/**
 * @brief Muestra la pantalla de limpieza de la bomba
 * 
 */
void pantallaLimpieza();

/**
 * @brief Consulta si se presionó el boton de finalizar
 * 
 * @return int 
 *  @li @c 0 No se presionó ningún boton
 *  @li @c 4 Botón de finalizar
 */
int tactilLimpieza();

/**
 * @brief Muestra la pantalla con los datos para conectarse al WiFi
 * 
 */
void pantallaWIFI ();

/**
 * @brief Consulta si se presionó el boton de finalizar
 * 
 * @return int 
 *  @li @c 0 No se presionó ningún boton
 *  @li @c 4 Botón de finalizar
 */
int tactilWIFI ();

void imprimirFinalizando();

void imprimirError();

void imprimirGuardando();

void pantallaCalibrarA();

void pantallaCalibrarB();

void pantallaCalibrarC();

/**
 * @brief Muestra la pantalla del agitador
 * 
 */
void pantallaAgitador();

/**
 * @brief Activa o desactiva agitador
 * 
 * @return int
 *  @li @c 1 Activa agitador
 *  @li @c 2 Desactiva agitador
 *  @li @c 4 Botón de finalizar
 */
int tactilAgitador();

/**
 * @brief Muestra mensaje en pantalla ON/OFF agitador
 * 
 */
void estadoAgitador(byte);

/**
 * @brief Muestra el volumen en el punto final
 * 
 */
void imprimirResultado(float);

#endif