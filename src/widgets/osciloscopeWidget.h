

#ifndef Osc_WIDGET_H
#define Osc_WIDGET_H

// Definiciones generales (hay que hacer cambios en orden)
// -----------------------
#define NUM_POINTS SCREEN_WIDTH    // Número de puntos para la gráfica (ancho de la pantalla)
#define firstEspacioSup 10         // Espacio superior para la grilla
#define freq_position 6            // Posicion del valor de frecuencia (recordar cuenta inicia en 0)
#define n_osc_chanels 6            // Número de canales del osciloscopio

// Ajustes de Widget (deben moverse a una clase de sensores)
//#define FirstFactorSensor 30.0
//#define FirstValorMinLecturaRMS 0.25
//#define FirstFactorAmplificador 10

// Ajustes medicion RMS
//#define FirstNUM_MUESTRAS 200      // Número de muestras (ajustable)
//#define FirstTIEMPO_LECTURA 100    // Duración de la lectura en milisegundos
//#define FirstEsperaUs 100          // Espera entre cada medicion en microsegundos

#include "externalCode\EasySensor\EasySensor.h"
#include "additionalDrawingFun.h"                            // Funciones de dibujo adicionales, ya que no las incluye Adafruit_SSD1306.h
#include "externalCode/EasyElectricCalc/EasyElectricCalc.h"  // Funciones para cálculos eléctricos

// Widget de osciloscopio
class OsciloscopeWidget: public Widget{
protected:
    EasySensor * sensors;  // Sensor asociado al widget
    char * letters;  // Letra que identifica cada medición
    int currentChanel = 0;  // Entrada actual

    // Variables ajustables de sensor
    //float factorSensor = FirstFactorSensor;         // Factor del sensor (ajustable)
    //float valorMinLecturaRMS = FirstValorMinLecturaRMS;    // Valor mas bajo que se puede registrar. Para eliminar ruido.
    //float factorAmplificador = FirstFactorAmplificador;      // Ajusta el valor según la amplificación del ccircuito de entrada

    // Variables ajustables de medición RMS
    //const int NUM_MUESTRAS = FirstNUM_MUESTRAS;      // Número de muestras (ajustable)
    //const int TIEMPO_LECTURA = FirstTIEMPO_LECTURA;    // Duración de la lectura en milisegundos
    //const int esperaUs = FirstEsperaUs;          // Espera entre cada medicion en microsegundos

    // Variables de trabajo
    float * voltajes;            //float voltajes[FirstNUM_MUESTRAS]; // Array para almacenar los valores del ADC
    unsigned long * times;       //unsigned long times[FirstNUM_MUESTRAS]; // Array para almacenar los tiempos de lectura
    int oscValues[NUM_POINTS]; // Array para almacenar los valores reflejados en pantalla
    int xPos = 0;              // Posición x actual para dibujar en la pantalla
    float offset = 0.0;
    float outputValue = 0.0;
    float outputValue2 = 0.0;
private:
    void measureData();  // Proceso para medir los datos
    void drawBackground(char * variableName, float variableValue, char * variableUnits,  float variableValue2 = 0.0, char * variableUnits2 = NULL);  // Agregar lineas de fondo
    void drawMesurement();  // Dibujar medición

public:
    OsciloscopeWidget(Adafruit_SSD1306 * display, botones * botonesUI_w, EasySensor * sensors_w, char * inLetters): Widget(display, botonesUI_w), sensors(sensors_w), letters(inLetters) {};
    //~OsciloscopeWidget(); // genera error en la compilación

    //void draw();
    char update();
};


char OsciloscopeWidget::update() {
    // Proceso de actualización de datos
    if(active){
        display->clearDisplay();                   // Borrar la pantalla antes de redibujar el buffer
        measureData();                             // Proceso para medir el dato
        drawBackground("Irms", outputValue, "A", outputValue2, "Hz");  // Agregar lineas de fondo y el resultado de la medición
        drawMesurement();                          // Dibujar linea de medición

        draw();                                    // aplicar acciones de dibujo si son requeridas (envia el buffer al display)
        char controles = controls();               // aplicar acciones de control si son requeridas y retorna char de resultado
        if(controles == '1' && currentChanel < n_osc_chanels-1){
            currentChanel += 1;  // Cambiar el canal de medición
            Serial.println(currentChanel);
        }
        else if (controles == '2' && currentChanel > 0){
            currentChanel -= 1;  // Cambiar el canal de medición
            Serial.println(currentChanel);
        }

        return controls();                         // aplicar acciones de control si son requeridas y retorna char de resultado
    }
}

// Proceso para medir los datos
void OsciloscopeWidget::measureData(){
    /*
    valorRMS = medirValorRMS(25,voltajes,offset);
    Serial.println(valorRMS);

    // Caluclar el valor de salida con los ajustes necesarios segun el tipo de sensor
    outputValue = calcularCorrienteRMS(valorRMS, offset, factorSensor, factorAmplificador);  // Debe cambiar segun el tipo de sensor
    */

    // Actualizar el valor de la medición
    outputValue =  sensors->getSensorData()[currentChanel];      // currentChanel es el canal elegido, en este caso hay 6 y el setimo es la frecuencia
    outputValue2 = sensors->getSensorData()[freq_position];      // currentChanel es el canal elegido, en este caso hay 6 y el setimo es la frecuencia

    voltajes = sensors->getSensorChanelRawData(currentChanel);
    times = sensors->getSensorChanelRawTimes(currentChanel);
    offset = sensors->getSensorChanelOffset(currentChanel);   // Obtener el offset del sensor en el canal actual luego de haber hecho las lecturas

    // Mostrar los valores de voltajes y tiempos
    /*
    Serial.print("Times: ");
    for(int i = 0; i < NUM_MUESTRAS; i++){
        Serial.print(times[i]);
        Serial.print(";");
    }
    Serial.println();
    */
}

// Agregar lineas de fondo
void OsciloscopeWidget::drawBackground(char * variableName, float variableValue, char * variableUnits, float variableValue2, char * variableUnits2){
    // Agregar datos en pantalla
    // ------------------------------------------
    display->setTextSize(1);      // Tamaño del texto (1 es el más pequeño, se puede aumentar)
    display->setTextColor(SSD1306_WHITE); // Color del texto (SSD1306_WHITE para blanco)
    display->setCursor(0, 0);     // Posición del cursor en la esquina superior izquierda
    display->print(variableName);
    display->print(":");
    display->print(variableValue);
    //display->print(" ");
    display->print(variableUnits);

    // Agregar segundo valor si es necesario
    if(variableUnits2 != NULL){
        display->print(" at");
        display->print(variableValue2);
        //display->print(" ");
        display->print(variableUnits2);
    }

    // Indicar el canal atual
    display->setCursor(4, 57);     // Posición del cursor en la esquina superior izquierda
    display->print("Canal:");
    display->print(this->currentChanel);

    // Dibujar frecuencia, canal actual y otros datos
    // ------------------------------------------
    // por hacer


    // Agregar lineas de diseño
    //------------------------------------------

    // Configurar grilla
    int espacioSup = firstEspacioSup;
    int sizeH = NUM_POINTS;
    int sizeV = SCREEN_HEIGHT-espacioSup;   // Pendiente: Hay que corregir definicion de SCREEN_HEIGHT para que no dependa del exterior

    int n_verticales = 5;   // Cantidad de grillas verticales
    int n_horizontales = 4; // Cantidad de grillas horizontales

    // Grilla vertical
    for(uint8_t i=n_verticales-1; i>0; i--){
        int x_line = sizeH - i*(sizeH/n_verticales);
        drawDashedLine2(x_line, espacioSup, x_line, sizeV + espacioSup, 2, display);
    }

    // Grilla horizontal
    for(uint8_t i=n_horizontales-1; i>0; i--){
        int y_line = sizeV - i*(sizeV/n_horizontales) + espacioSup;
        drawDashedLine2(0, y_line, sizeH, y_line, 2, display);
    }
}

// Dibujar medición
void OsciloscopeWidget::drawMesurement(){

    // Crear valores que se mostrarán en pantalla
    for(int i = 0; i < NUM_POINTS; i++){
        oscValues[i] = SCREEN_HEIGHT - (voltajes[i]-offset+1.55)*(SCREEN_HEIGHT/3.4);  // La ecuacion tambien centra el valor   
        xPos = i;
    }

    // Dibujar la línea de osciloscopio
    for (int i = 0; i < NUM_POINTS - 1; i++) {
      int x1 = i;
      int y1 = oscValues[i];
      int x2 = i + 1;
      int y2 = oscValues[i + 1];
      if(i<=xPos) display->drawLine(x1, y1, x2, y2, SSD1306_WHITE);
    }

}

#endif // Osc_WIDGET_H