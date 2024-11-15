/*=============================================================================
 * Autor: Fernando Ezequiel Daniele <fernandodaniele1993@gmai.com>
 * Fecha: 2020/12/21
 *===========================================================================*/

/*=============================================================================
 * Inclusiones de archivos y bibliotecas
 *===========================================================================*/
#include <Adafruit_GFX.h>  //librería gráfica para TFT
#include <MCUFRIEND_kbv.h> //librería especifica del módulo
#include "pantallaTFT.h"
#include "panelTactil.h"
#include "uart.h"

/*=============================================================================
 * Definiciones y macros
 *===========================================================================*/
#define BLACK   0x0000    //Colores predefinidos. Se pueden agregar o modificar
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
//#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define TXT_BTN_SIZE    2
#define CALIBRAR_BTN    "CALIBRAR"
#define TITULAR_BTN     "TITULAR"
#define CONFIGURAR_BTN  "AJUSTES"
#define WIFI_BTN        "WIFI"
#define B1_BTN          "PH 4"
#define B2_BTN          "PH 7"
#define B3_BTN          "PH 10"
#define REGRESAR_BTN    "VOLVER"
#define FINALIZAR       "FIN"
#define GUARDAR         "GUARDAR"
#define T_INC_DEC        70
#define RETARDO_PANTALLA 1000
#define RETARDO_PIXEL    5000
/*=============================================================================
 * Variables y objetos locales
 *===========================================================================*/
MCUFRIEND_kbv tft;        //Objeto pantalla gráfica

Adafruit_GFX_Button unoBtn, dosBtn, tresBtn, cuatroBtn, cincoBtn, seisBtn;

char bufferA[6], bufferB[6], bufferC[6]; //variables de calibración en bits formato decimal (0-0V 2047-3.3V)
float bufferAPH = 4.0, bufferBPH = 7.0, bufferCPH = 10.0; //variables de calibración en ph
int pixel_x, pixel_y;   
int volumenCorte = 10;
int volumenInyectar = 10;

uint16_t gX, gY; //variables para la curva de titulacion
float ph;
unsigned long T;

/*=============================================================================
 * Definición de funciones globales públicas
 *===========================================================================*/
 //Inicializa el display TFT
 void inicializarTFT(){
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3)
  ID = 0x9486; // write-only shield
  tft.begin(ID);
  tft.setRotation(3); //horizontal 1 o 3
}

void pantallaNegra(){
  tft.fillScreen(BLACK);  //borra cualquier imagen previa
  tft.drawRect(8, 8, 312, 232, WHITE); //(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
}

void dibujarBotones(char *a, char *b, char *c, char *d){
  tft.setTextColor(WHITE);
                // (gfx, x, y, w, h, outline, fill, textcolor, nombre, textsize)
  unoBtn.initButton(&tft,  84, 150, 140, 40, WHITE, CYAN, BLACK, a, TXT_BTN_SIZE); 
  dosBtn.initButton(&tft, 240, 150, 140, 40, WHITE, CYAN, BLACK, b, TXT_BTN_SIZE);
  tresBtn.initButton(&tft,  84, 210, 140, 40, WHITE, CYAN, BLACK, c, TXT_BTN_SIZE);
  cuatroBtn.initButton(&tft, 240, 210, 140, 40, WHITE, CYAN, BLACK, d, TXT_BTN_SIZE);
  unoBtn.drawButton(false);//dibuja boton. false es para intercambiar colores
  dosBtn.drawButton(false);
  tresBtn.drawButton(false);
  cuatroBtn.drawButton(false);
}

//Muestra la pantalla de inicio
void pantallaInicial(){
  pantallaNegra();  //borra cualquier imagen previa

  dibujarBotones(CALIBRAR_BTN, TITULAR_BTN, CONFIGURAR_BTN, WIFI_BTN); //Se inician y muestran los botones
  tft.drawRect(8, 8, 312, 110, WHITE);
  //Se dibuja un rectangulo para escribir dentro
  tft.setCursor(100,13);
  tft.setTextColor(WHITE);
  tft.print("TITULADOR");
  tft.setCursor(100,38);
  tft.print("AUTOMATICO");
  tft.setCursor(15,61);
  tft.print("Desarrollado por");
  tft.setCursor(15,86);
  tft.print("U.T.N. San Francisco.");
}

//Consulta si se presionó algún botón en la pantalla inicial
int consultaTactil(){
  //Consulta si se presionó el tactil y guarda las coordenadas
  bool presionado = Touch_getXY(&pixel_x, &pixel_y);
  
  //Compara el dato obtenido con los botones de la pantalla
  unoBtn.press(presionado && unoBtn.contains(pixel_x, pixel_y));
  dosBtn.press(presionado && dosBtn.contains(pixel_x, pixel_y));
  tresBtn.press(presionado && tresBtn.contains(pixel_x, pixel_y));
  cuatroBtn.press(presionado && cuatroBtn.contains(pixel_x, pixel_y));

  //Si alguno de los botones fue soltado, retorna el valor correspondiente
  if (unoBtn.justReleased()) {
    return 1;
  }
  else if (dosBtn.justReleased()) {
    return 2; 
  }
  else if (tresBtn.justReleased()) {
    return 3;
  }
  else if (cuatroBtn.justReleased()) {
    return 4; 
  }
  else{
    return 0;
  } 
}

//Muestra la pantalla con el valor actual de cada buffer y da la opción de elegir 
//cada uno de ellos por separado para realizar la calibración
void pantallaElegirBuffer(){
    //Limpia la pantalla y muestra el valor actual de los buffers
    pantallaNegra();
    tft.setTextColor(WHITE);
    tft.setCursor(10,20);
    tft.println(" Coloque el electrodo\n  en el buffer\n");
    tft.print("  Seleccione la opcion\n  correspondiente al\n   buffer utilizado");

    //Dibuja los botones correspondientes a esta pantalla
    dibujarBotones(B1_BTN, B2_BTN, B3_BTN, REGRESAR_BTN); //Se inician y muestran los botones
}

//Muestra la pantalla con el valor actual de ph que será guardado como valor para calibrar el instrumento
void pantallaCalibrar (){
  pantallaNegra();
  dosBtn.initButton(&tft,  84, 210, 140, 40, WHITE, CYAN, BLACK, REGRESAR_BTN, TXT_BTN_SIZE);
  unoBtn.initButton(&tft, 240, 210, 140, 40, WHITE, CYAN, BLACK, GUARDAR, TXT_BTN_SIZE);
  dosBtn.drawButton(false);
  unoBtn.drawButton(false);
  tft.setTextColor(WHITE);
  tft.setCursor(10,10);
  tft.println(" Realice la medicion\n");
  tft.println("  Presione guardar cuando\n");
  tft.println("  el valor se estabilice");
}

//Consulta si se presionó algún botón en la pantalla de elección de buffer
int tactilCalibrar(){
  float tempMV = ph;
  //Acá tengo que leer el valor del electrodo desde el ESP
  if(millis()>T+500){
    if(leerPotencial(&ph)==0){
        imprimirError();
        return 4;
    }
    else
    {
    tft.setCursor(100,110);
    tft.setTextColor(BLACK);
    tft.print(tempMV);
    tft.print(" pH");
    //Acá tengo que mostrar en pantalla ese valor leido
    tft.setCursor(100,110);
    tft.setTextColor(WHITE);
    tft.print(ph);
    tft.print(" pH");
    }
  }

  bool  ab = Touch_getXY(&pixel_x, &pixel_y);
  unoBtn.press(ab && unoBtn.contains(pixel_x, pixel_y));
  dosBtn.press(ab && dosBtn.contains(pixel_x, pixel_y));

  if (unoBtn.justReleased()){
    return 3;
  }
  else if (dosBtn.justReleased()){
    return 4;
  }
  else{
    return 0;
  }
}

//Muestra la pantalla con la curva de titulación y el valor actual de pH
void pantallaMedir(){
 
  pantallaNegra(); 
  unoBtn.initButton(&tft, 240, 210, 140, 40, WHITE, CYAN, BLACK, FINALIZAR, TXT_BTN_SIZE);
  unoBtn.drawButton(false);

  //Dibuja el grafico
  tft.drawLine(25, 30, 25, 160, WHITE);
  tft.drawLine(25, 160, 310, 160, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(1); 
  tft.setCursor(10,15);
  tft.print("pH");
  tft.setCursor(10,30);
  tft.print("14");
  tft.setCursor(10,91);
  tft.print(" 7");
  tft.setCursor(10,153);
  tft.print(" 0");
  tft.setCursor(25,163);
  tft.print("0");
  tft.setCursor(280,163);
  tft.print("t");
  tft.setTextSize(2);

  T= millis();
  gX = 25;
}

//Actualiza el grafico y consulta si se presionó el boton de finalizar
int tactilMedir(){
  
  if(millis()>T+RETARDO_PIXEL){

    float tmpPH = ph;
    //Acá debería consultar al ESP el valor del pH
    if(leerPotencial(&ph)==0){
      imprimirError();
      return 4;
    }

    gY =(ph * (30 - 160)/14+ 160);
    tft.drawPixel(gX,gY,YELLOW);

    tft.setCursor(25,200);
    tft.setTextColor(BLACK);
    tft.print(tmpPH);
    tft.print(" pH");
    tft.setCursor(25,200);
    tft.setTextColor(WHITE);
    tft.print(ph);
    tft.print(" pH");
    T = millis();
    gX++;
  }
  
  bool  d = Touch_getXY(&pixel_x, &pixel_y);
  unoBtn.press(d && unoBtn.contains(pixel_x, pixel_y));
  
  if (unoBtn.justReleased()){
    return 4;
  }
  else{
    return 0;
  }

}

//Muestra la pantalla con la opciones para configurar los buffers a utilizar, el volumen de corte y para ejecutar la limpieza de la bomba
void pantallaAjustes()
{
    pantallaNegra();
    
    tft.setCursor(10,20);
    tft.setTextColor(WHITE); 
    tft.print ("Seleccione una opcion");
    dibujarBotones("VOLUMEN", "LIMPIEZA", "AGITADOR", REGRESAR_BTN);
}

//Muestra la pantalla que permite ajustar el valor del volumen de corte
void pantallaVolumenCorte()
{
  pantallaNegra();
  
  tft.setTextColor(WHITE);
  tft.setCursor(10,15);
  tft.print("Configure el volumen\n  de corte");
  tft.setCursor(10,65);
  if(leerVolumenCorte(&volumenCorte) == 0)  {
    tft.print("E");
  }
  else  {
  tft.print(" Volumen:");
  tft.setCursor(125,65);
  tft.print(volumenCorte);
  tft.print(" mL");
  }

  dibujarBotones("+", "-", GUARDAR, REGRESAR_BTN);
 }

//Permite configurar el volumen de corte y consulta si se presionó el boton de guardar
int  tactilVolumenCorte()
{
  bool  presionado = Touch_getXY(&pixel_x, &pixel_y);
  unoBtn.press(presionado && unoBtn.contains(pixel_x, pixel_y));
  dosBtn.press(presionado && dosBtn.contains(pixel_x, pixel_y));
  tresBtn.press(presionado && tresBtn.contains(pixel_x, pixel_y));
  cuatroBtn.press(presionado && cuatroBtn.contains(pixel_x, pixel_y));  
  if (unoBtn.justReleased()){
    delay(T_INC_DEC); 
    tft.setCursor(125,65);
    tft.setTextColor(BLACK);
    tft.print(volumenCorte);
    volumenCorte = volumenCorte + 1;
    tft.setTextColor(WHITE);
    tft.setCursor(125,65);
    tft.print(volumenCorte);
  }
  else if (dosBtn.justReleased()){
    delay(T_INC_DEC);
    tft.setCursor(125,65);
    tft.setTextColor(BLACK);
    tft.print(volumenCorte);
    volumenCorte = volumenCorte - 1;
    tft.setTextColor(WHITE);
    tft.setCursor(125,65);
    tft.print(volumenCorte);
  }
  else if (tresBtn.justReleased()){
    imprimirGuardando();
    char volumenStr [6];
    sprintf(volumenStr, "%d", volumenCorte);
    if(guardarVolumenCorte(volumenStr)==0){
      imprimirError();
    }
    return 4;
  }
  else if (cuatroBtn.justReleased()){
    return 4;
  }
  else
  {
    return 0;
  }
}

//Muestra la pantalla de limpieza de la bomba
void pantallaLimpieza ()
{
  pantallaNegra();
  tft.setTextColor(WHITE);
  tft.setCursor(10,15);
  tft.print("Volumen a inyectar: ");
  
  unoBtn.initButton(&tft,  84, 90, 140, 40, WHITE, CYAN, BLACK, "+", TXT_BTN_SIZE); 
  dosBtn.initButton(&tft, 240, 90, 140, 40, WHITE, CYAN, BLACK, "-", TXT_BTN_SIZE);
  tresBtn.initButton(&tft,  84, 150, 140, 40, WHITE, CYAN, BLACK, "INYECTAR", TXT_BTN_SIZE);
  cuatroBtn.initButton(&tft, 240, 150, 140, 40, WHITE, CYAN, BLACK, REGRESAR_BTN, TXT_BTN_SIZE);
  cincoBtn.initButton(&tft,  84, 210, 140, 40, WHITE, CYAN, BLACK,"LIMPIAR", TXT_BTN_SIZE);
  seisBtn.initButton(&tft, 240, 210, 140, 40, WHITE, CYAN, BLACK,FINALIZAR, TXT_BTN_SIZE);
  unoBtn.drawButton(false);//dibuja boton. false es para intercambiar colores
  dosBtn.drawButton(false);
  tresBtn.drawButton(false);
  cuatroBtn.drawButton(false);
  cincoBtn.drawButton(false);
  seisBtn.drawButton(false);
}

//Permite realizar limpieza
int  tactilLimpieza()
{
  bool  presionado = Touch_getXY(&pixel_x, &pixel_y);
  unoBtn.press(presionado && unoBtn.contains(pixel_x, pixel_y));
  dosBtn.press(presionado && dosBtn.contains(pixel_x, pixel_y));
  tresBtn.press(presionado && tresBtn.contains(pixel_x, pixel_y));
  cuatroBtn.press(presionado && cuatroBtn.contains(pixel_x, pixel_y));
  cincoBtn.press(presionado && cincoBtn.contains(pixel_x, pixel_y));
  seisBtn.press(presionado && seisBtn.contains(pixel_x, pixel_y));  
  if (unoBtn.justReleased()){
    delay(T_INC_DEC); 
    tft.setCursor(145,45);
    tft.setTextColor(BLACK);
    tft.print(volumenInyectar);
    tft.print(" mL");
    volumenInyectar = volumenInyectar + 1;
    tft.setTextColor(WHITE);
    tft.setCursor(145,45);
    tft.print(volumenInyectar);
    tft.print(" mL");
  }
  else if (dosBtn.justReleased()){
    delay(T_INC_DEC);
    tft.setCursor(145,45);
    tft.setTextColor(BLACK);
    tft.print(volumenInyectar);
    tft.print(" mL");
    volumenInyectar = volumenInyectar - 1;
    tft.setTextColor(WHITE);
    tft.setCursor(145,45);
    tft.print(volumenInyectar);
    tft.print(" mL");
  }
  else if (tresBtn.justReleased()){
    char volumenStr [6];
    sprintf(volumenStr, "%d", volumenInyectar);
    if(InyectarVolumen(volumenStr)==0)
    {
      imprimirError();
      return 4;
    }
    return 3;
  }
  else if (cuatroBtn.justReleased()){
    return 4;
  }
  else if (cincoBtn.justReleased()){
    return 5;
  }
  else if (seisBtn.justReleased()){
    return 6;
  }
  else{
    return 0;
  }
}

//Muestra la pantalla con los datos para conectarse al WiFi
void pantallaWIFI ()
{
  pantallaNegra();
  
  unoBtn.initButton(&tft,  240, 210, 140, 40, WHITE, CYAN, BLACK, REGRESAR_BTN, 2); // (gfx, x, y, w, h, outline, fill, textcolor, "Calibracion", textsize)
  unoBtn.drawButton(false);
  tft.setTextColor(WHITE);
  tft.setCursor(25,20);
  tft.println("Red: ESP32_AP");
  tft.setCursor(25,65);
  tft.println("Clave: 12345678");
  tft.setCursor(25,110);
  tft.println("Web: 192.168.4.1/");
}

//Consulta si se presionó el boton de finalizar
int tactilWIFI()
{
  bool  presionado = Touch_getXY(&pixel_x, &pixel_y);
  unoBtn.press(presionado && unoBtn.contains(pixel_x, pixel_y));
  if (unoBtn.justReleased()){
    return 4;
  }
  else{
    return 0;
  }
}

void imprimirFinalizando()
{
  tft.setCursor(15,100);
  pantallaNegra();
  tft.setTextColor(WHITE);
  tft.print("Finalizando...");
  delay(RETARDO_PANTALLA);
}

void imprimirError(){
  tft.setCursor(15,100);
  pantallaNegra();
  tft.setTextColor(WHITE);
  tft.print("ERROR");
  delay(RETARDO_PANTALLA);
}

void imprimirGuardando()
{
  pantallaNegra();
  tft.setCursor(15,100);
  tft.setTextColor(WHITE);
  tft.print("Guardando...");
  delay(RETARDO_PANTALLA);
}

void imprimirResultado(float resultado)
{
  tft.setCursor(15,100);
  
  pantallaNegra();
  tft.setTextColor(WHITE);
  tft.print("Volumen = ");
  tft.print(resultado);
  tft.print(" mL ");
  delay(RETARDO_PANTALLA*5);
}

void pantallaCalibrarA()
{
  imprimirGuardando();
  //Cuando se de guardar hay que enviar un señal para que se guarde ese valor
  if(calibrarBufferA()==0)  {
     imprimirError();
  }
  else{
      //leer valor guardado y mostrar en pantalla
  }
}

void pantallaCalibrarB()
{
  imprimirGuardando();
  //Cuando se de guardar hay que enviar un señal para que se guarde ese valor
  if(calibrarBufferB()==0){
     imprimirError();
  }
  else{
      //leer valor guardado y mostrar en pantalla
  }
}

void pantallaCalibrarC()
{
  imprimirGuardando();
  //Cuando se de guardar hay que enviar un señal para que se guarde ese valor
  if(calibrarBufferC()==0){
    imprimirError();
  }
  else{
      //leer valor guardado y mostrar en pantalla
  }
}

void pantallaAgitador()
{
    pantallaNegra();
    tft.setTextColor(WHITE);
    tft.setCursor(10,20); 
    tft.println ("Seleccione una opcion.");
    dibujarBotones("ON", "OFF", " ", REGRESAR_BTN);
}

int tactilAgitador(){
    bool  presionado = Touch_getXY(&pixel_x,&pixel_y);
    unoBtn.press(presionado && unoBtn.contains(pixel_x, pixel_y));
    dosBtn.press(presionado && dosBtn.contains(pixel_x, pixel_y));
    cuatroBtn.press(presionado && cuatroBtn.contains(pixel_x, pixel_y));
    if (unoBtn.justReleased()){
      return 1;
    }
    else if (dosBtn.justReleased()){
      return 2;
    }
    else if (cuatroBtn.justReleased()){
      return 4; 
    }
    else{
      return 0;
    }
  
}

void estadoAgitador(byte a)
{
    pantallaNegra();
    tft.setTextColor(WHITE);
    tft.setCursor(20,20); 
    tft.print ("Agitador ");
    if(a){
      tft.print("ON");
    }
    else{
      tft.print("OFF");
    }
    delay(RETARDO_PANTALLA);
}
