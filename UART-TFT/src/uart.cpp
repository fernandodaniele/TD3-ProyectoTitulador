/*=============================================================================
 * Autor: Fernando Ezequiel Daniele <fernandodaniele1993@gmai.com>
 * Fecha: 2020/12/21
 *===========================================================================*/

#include "uart.h"

#define INICIO_CAL          'A' 		
#define FIN_CAL    		    'B'
#define LECTURA_POTENCIAL   'C'
#define GRABA_BUFFER4       'D'
#define GRABA_BUFFER7       'E'
#define GRABA_BUFFER10      'F'
#define INICIO_TIT          'G'
#define LECTURA_PH          'H'
#define CANCELA_TIT         'I'
#define FIN_TIT             "J"
#define INICIO_LIMPIEZA     'K'
#define FIN_LIMPIEZA        'L'
#define LEE_VOLUMEN         'M'
#define GUARDA_VOLUMEN      'N'
#define ESTADO_AGITADOR     'O'
#define HABILITA_AGIT	    'P'
#define DESHABILITA_AGIT    'Q'
#define INYECTA_VOLUMEN     'R'
#define BARRA               '/'

void iniciarUart (){
    Serial.begin(115200);
}

byte confirmacion(){
    unsigned long tOut = millis ();
    while(Serial.available()==0){
        if(millis()> (tOut + 1000)){
            return 0;
        }
    }
    String ack = Serial.readStringUntil(BARRA);
    if (ack == "K"){
        return 1;   //se escribió correctamente
    }
    else{
        return 0;   //hubo error
    }
}  

byte estadoTitulacion(float *val){
    unsigned long tOut = millis ();
    while(Serial.available()==0){
        if(millis()> (tOut + 500)){
            return 0;
        }
    }
    String ack = Serial.readStringUntil(BARRA);
    if (ack == FIN_TIT){
        String temp = Serial.readStringUntil(BARRA);
        *val = temp.toFloat();
        return 1;   //finalizo la titulacion
    }
    else{
        return 0;   //continua la titulacion
    } 
}

byte iniciarCalibracion(){
    Serial.write(INICIO_CAL);    //envia comando
    return confirmacion();  
}

byte finalizarCalibracion(){
    Serial.write(FIN_CAL);    //envia comando
    return confirmacion();  
}

byte leerPotencial(float *val){
    Serial.write(LECTURA_POTENCIAL);    //envia comando
    unsigned long tOut = millis ();
    while(Serial.available()==0){
        if(millis()> (tOut + 1000)){
            return 0;
        }
    }
    String temp = Serial.readStringUntil(BARRA);
    *val = temp.toFloat();
    return 1;
}

byte calibrarBufferA(){
    Serial.write(GRABA_BUFFER4);
    return confirmacion();    
}

byte calibrarBufferB(){
    Serial.write(GRABA_BUFFER7);
    return confirmacion();    
}

byte calibrarBufferC(){
    Serial.write(GRABA_BUFFER10);
    return confirmacion();    
}

byte iniciarTitulacion(){
    Serial.write(INICIO_TIT);
    return confirmacion();    
}

byte cancelarTitulacion(){
    Serial.write(CANCELA_TIT);
    return confirmacion();    
}

byte iniciarLimpieza(){
    Serial.write(INICIO_LIMPIEZA);
    return confirmacion();           
}

byte finalizarLimpieza(){
    Serial.write(FIN_LIMPIEZA);
    return confirmacion();       
}

byte leerVolumenCorte(int *val){
    Serial.write(LEE_VOLUMEN);    //envia comando para leer el valor de buffer A
    Serial.flush();
    unsigned long tOut = millis ();
    while(Serial.available()==0)
    {
        if(millis()> (tOut + 1000))
        {
            return 0;
        }
    }
    String temp = Serial.readStringUntil(BARRA);
    *val = temp.toInt();
    return 1;
}

byte guardarVolumenCorte(char * val){
    Serial.write(GUARDA_VOLUMEN);
    Serial.write(val);
    return confirmacion();    
}

byte InyectarVolumen(char * val){
    Serial.write(INYECTA_VOLUMEN);
    Serial.write(val);
    return confirmacion();    
}

byte habilitarAgitador(){
    Serial.write(HABILITA_AGIT);
    return confirmacion();
      
}

byte deshabilitarAgitador(){
    Serial.write(DESHABILITA_AGIT);
    return confirmacion();    
}