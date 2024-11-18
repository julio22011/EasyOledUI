

#ifndef Osc_WIDGET_H
#define Osc_WIDGET_H

// Definiciones generales (hay que hacer cambios en orden)
// -----------------------
#define NUM_POINTS SCREEN_WIDTH    // Número de puntos para la gráfica (ancho de la pantalla)

// Ajustes de Widget (deben moverse a una clase de sensores)
#define FirstFactorSensor 30.0
#define FirstValorMinLecturaRMS 0.25
#define FirstFactorAmplificador 10

// Ajustes medicion RMS
#define FirstNUM_MUESTRAS 500      // Número de muestras (ajustable)
#define FirstTIEMPO_LECTURA 100    // Duración de la lectura en milisegundos
#define FirstEsperaUs 100          // Espera entre cada medicion en microsegundos


#include "additionalDrawingFun.h"                            // Funciones de dibujo adicionales, ya que no las incluye Adafruit_SSD1306.h
#include "externalCode/EasyElectricCalc/EasyElectricCalc.h"  // Funciones para cálculos eléctricos

// Widget de osciloscopio
class OsciloscopeWidget: public Widget{
protected:
    int * inputs;
    char * letters;  // Letra que identifica cada medición
    int currentInput = 0;  // Entrada actual

    // Variables ajustables de sensor
    float factorSensor = FirstFactorSensor;         // Factor del sensor (ajustable)
    float valorMinLecturaRMS = FirstValorMinLecturaRMS;    // Valor mas bajo que se puede registrar. Para eliminar ruido.
    float factorAmplificador = FirstFactorAmplificador;      // Ajusta el valor según la amplificación del ccircuito de entrada

    // Variables ajustables de medición RMS
    const int NUM_MUESTRAS = FirstNUM_MUESTRAS;      // Número de muestras (ajustable)
    const int TIEMPO_LECTURA = FirstTIEMPO_LECTURA;    // Duración de la lectura en milisegundos
    const int esperaUs = FirstEsperaUs;          // Espera entre cada medicion en microsegundos

    // Variables de trabajo
    float voltajes[FirstNUM_MUESTRAS]; // Array para almacenar los valores del ADC
    int oscValues[NUM_POINTS]; // Array para almacenar los valores reflejados en pantalla
    int xPos = 0;              // Posición x actual para dibujar en la pantalla
    float offset = 0.0;
    float valorRMS = 0.0;
    float outputValue = 0.0;
private:
    void measureData();  // Proceso para medir los datos
    void drawBackground(char * variableName, float variableValue, char * variableUnits);  // Agregar lineas de fondo
    void drawMesurement();  // Dibujar medición

public:
    OsciloscopeWidget(Adafruit_SSD1306 * display, botones * botonesUI_w, int * inputPins, char * inLetters): Widget(display, botonesUI_w), inputs(inputPins), letters(inLetters) {};
    //~OsciloscopeWidget(); // genera error en la compilación

    //void draw();
    char update();
};


char OsciloscopeWidget::update() {
    // Proceso de actualización de datos
    if(active){
        display->clearDisplay();                   // Borrar la pantalla antes de redibujar el buffer
        measureData();                             // Proceso para medir el dato
        drawBackground("Irms", outputValue, "A");  // Agregar lineas de fondo y el resultado de la medición
        drawMesurement();                          // Dibujar linea de medición

        draw();                                    // aplicar acciones de dibujo si son requeridas (envia el buffer al display)

        return controls();                         // aplicar acciones de control si son requeridas y retorna char de resultado
    }
}

// Proceso para medir los datos
void OsciloscopeWidget::measureData(){
    valorRMS = medirValorRMS(25,voltajes,offset);
    Serial.println(valorRMS);

    // Caluclar el valor de salida con los ajustes necesarios segun el tipo de sensor
    outputValue = calcularCorrienteRMS(valorRMS, offset, factorSensor, factorAmplificador);  // Debe cambiar segun el tipo de sensor
}

// Agregar lineas de fondo
void OsciloscopeWidget::drawBackground(char * variableName, float variableValue, char * variableUnits){
    // Agregar datos en pantalla
    // ------------------------------------------
    display->setTextSize(1);      // Tamaño del texto (1 es el más pequeño, se puede aumentar)
    display->setTextColor(SSD1306_WHITE); // Color del texto (SSD1306_WHITE para blanco)
    display->setCursor(0, 0);     // Posición del cursor en la esquina superior izquierda
    display->print(variableName);
    display->print(": ");
    display->print(variableValue);
    display->print(" ");
    display->print(variableUnits);

    // Agregar lineas de diseño
    //------------------------------------------

    // Configurar grilla
    int espacioSup = 10;
    int sizeH = NUM_POINTS;
    int sizeV = SCREEN_HEIGHT-espacioSup;

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
        oscValues[i] = SCREEN_HEIGHT - voltajes[i]*(SCREEN_HEIGHT/3.4);
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