/*=============================================================================
 * Autor: Fernando Ezequiel Daniele <fernandodaniele1993@gmai.com>
 * Fecha: 2020/12/21
 *===========================================================================*/
#include "uart.h"
#include "menu.h"
#include "pantallaTFT.h"

// Variable para el estado actual
pantalla_t pantalla;
byte opcion =0;
float resultado;

// Función para controlar errores de la MEF (Error handler)
void errorMEF( void ){
   imprimirError();
   iniciarMEF();
}

// Función para iniciar la MEF
void iniciarMEF( void ){
   pantalla = MENU_INICIAL; 
   inicializarTFT();
   pantallaInicial();
}

// Función para actualizar la MEF
void actualizarMEF( ){
   switch( pantalla ){
      case MENU_INICIAL:
         opcion = consultaTactil();
    	   switch (opcion){
            case 1:
               pantalla = MENU_ELEGIR_BUFFER;
               if(iniciarCalibracion()==0){   //envia señal para iniciar la calibración
                  imprimirError(); //Si da error, reinicia MEF
               }
               pantallaElegirBuffer();
               break;
            case 2:
               pantalla = MENU_TITULACION;
               pantallaMedir();
               if(iniciarTitulacion()==0){   //envia señal para iniciar la titulacion
                  imprimirError(); //Si da error, reinicia MEF
               }
               break;
            case 3:
               pantalla = MENU_AJUSTES;
               pantallaAjustes();
               break;
            case 4:
               pantalla = MENU_CONEXION;
               pantallaWIFI ();
               break;
            default:
               pantalla = MENU_INICIAL;
               break;
    	   }
    	   break;

      case MENU_ELEGIR_BUFFER:
         opcion = consultaTactil();
         switch (opcion){
            case 1:
               pantalla = MENU_CALIBRAR_A;
               pantallaCalibrar ();
               break;
            case 2:
               pantalla = MENU_CALIBRAR_B;
               pantallaCalibrar ();
               break;
            case 3:
               pantalla = MENU_CALIBRAR_C;
               pantallaCalibrar ();
               break;
            case 4:
               pantalla = MENU_INICIAL;
               if(finalizarCalibracion()==0){
                  imprimirError();
               }
               pantallaInicial();
               break;
            default:
               pantalla = MENU_ELEGIR_BUFFER;
               break;
    	   }
    	   break;

      case MENU_CALIBRAR_A:
         opcion = tactilCalibrar();
    	   if(opcion==3){
            if(calibrarBufferA()==0){
               imprimirError();
            }
            pantalla = MENU_ELEGIR_BUFFER;
            pantallaElegirBuffer();
    	   }
         else if(opcion==4){
            pantalla = MENU_ELEGIR_BUFFER;
            pantallaElegirBuffer();
    	   }
    	   break;

      case MENU_CALIBRAR_B:
         opcion =  tactilCalibrar();
    	   if(opcion==3){
            if(calibrarBufferB()==0){
               imprimirError();
            }
            pantalla = MENU_ELEGIR_BUFFER;
            pantallaElegirBuffer();
    	   }
         else if(opcion==4){
            pantalla = MENU_ELEGIR_BUFFER;
            pantallaElegirBuffer();
    	   }
    	   break;

      case MENU_CALIBRAR_C:
         opcion =  tactilCalibrar();
    	   if(opcion==3){
            if(calibrarBufferC()==0){
               imprimirError();
            }
            pantalla = MENU_ELEGIR_BUFFER;
            pantallaElegirBuffer();
    	   }
         else if(opcion==4){
            pantalla = MENU_ELEGIR_BUFFER;
            pantallaElegirBuffer();
    	   }
    	   break;

      case MENU_TITULACION:
         opcion = tactilMedir();
         
    	   if(opcion==4){
            if(cancelarTitulacion()){
              if(estadoTitulacion(&resultado)){
                 imprimirResultado(resultado);
               }
            }
            else{
              imprimirError();
            }
            pantalla = MENU_INICIAL;
            pantallaInicial();
    	   }
         //Se lee el puerto serie para ver si finalizó la titulación
         else if(estadoTitulacion(&resultado)) {
            imprimirResultado(resultado);
            pantalla = MENU_INICIAL;
            pantallaInicial();
         }
    	   break;

      case MENU_AJUSTES:
         opcion = consultaTactil();
         switch (opcion){   
            case 1:
               pantalla = MENU_VOLUMEN;
               pantallaVolumenCorte();
               break;
            case 2:
               pantalla = MENU_LIMPIEZA;
               pantallaLimpieza();
               break;
            case 3:
               pantalla = MENU_AGITADOR;
               pantallaAgitador();
               break;
            case 4:
               pantalla = MENU_INICIAL;
               pantallaInicial();
               break;
            default:
               pantalla = MENU_AJUSTES;
               break;
    	   }
         break;

      case MENU_VOLUMEN:
         opcion = tactilVolumenCorte();
    	   if(opcion==4){
            //Acá enviar al ESP el valor del volumen de corte
            pantalla = MENU_AJUSTES;
            pantallaAjustes();
    	   }
    	   break;

      case MENU_LIMPIEZA:
         opcion = tactilLimpieza();     
    	   if(opcion==4){
            pantalla = MENU_AJUSTES;
            pantallaAjustes();
    	   }
         else if(opcion==5){
            if(iniciarLimpieza()==0){
               imprimirError();
            }
         }
         else if(opcion==6){
            if(finalizarLimpieza()==0){
               imprimirError();
            }
         }
    	   break;

      case MENU_AGITADOR:
         opcion = tactilAgitador();
         if (opcion==0)
         {
            break;
         }
         else if(opcion==1){
            if(habilitarAgitador()){
               estadoAgitador(1);
            }
            else{
               iniciarMEF();
            }
         }
         else if(opcion==2){
            if(deshabilitarAgitador()){
               estadoAgitador(0);
            }
            else{
               iniciarMEF();
            }
         }
         pantalla = MENU_AJUSTES;
         pantallaAjustes();
    	   break;

      case MENU_CONEXION:
         opcion = tactilWIFI();
    	   if(opcion==4){
            pantalla = MENU_INICIAL;
            pantallaInicial();
    	   }
         break;

      default:
    	 errorMEF();
    	 break;
   }
}