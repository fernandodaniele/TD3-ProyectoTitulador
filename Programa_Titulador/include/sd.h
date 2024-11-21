#ifndef SD_H
#define SD_H

void inicializarSD();
int leeSD(char* dato);
int escribeSD(char* dato);
int escribeSDFloat(float dato);
int escribeSDInt(int dato);
void desmontarSD();

#endif

//ejemplo de guardado de tabla
/*
escribeSD("Nueva titulación\n");    // Cuando comienza la titulación 

//Guarda el resultado en la SD (faltaría en WiFi)   // Cuando finializa la titulaoción 
            escribeSD("Volumen en punto de equivalencia = ");
            escribeSDFloat(volumenFinal);
            escribeSD("\n");

            //guarda todos lo valores en la SD -- Esto no haría falta
            escribeSD("Volumen[mL]\tpH\t\tDerivada 1\t\tDerivada 2\n");
            
            for(int vol = 1; vol < (cont); vol++)
            {
                escribeSDFloat(volumenInyectado[vol]);
                escribeSD("\t\t");
                escribeSDFloat(titulacionPH[vol]);
                escribeSD("\t\t");
                if((vol<2)||(vol>(cont-2)))
                {
                    escribeSD("Sin dato\t\tSin dato");
                }
                else
                {
                    escribeSDFloat(derivada1 [vol]);
                    escribeSD("\t\t\t");
                    escribeSDFloat(derivada2 [vol]);
                }
                escribeSD("\n");  
            }
            escribeSD("Fin titulación\n\n");

*/