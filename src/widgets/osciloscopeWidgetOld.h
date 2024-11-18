
// Funcion para crear osciloscopio
#define NUM_POINTS SCREEN_WIDTH    // Número de puntos para la gráfica (ancho de la pantalla)
int adcValues[NUM_POINTS]; // Array para almacenar los valores del ADC
int xPos = 0;              // Posición x actual para dibujar en la pantalla

// Ajuste medicion RMS
const int NUM_MUESTRAS = 500;      // Número de muestras (ajustable)
const int TIEMPO_LECTURA = 100;    // Duración de la lectura en milisegundos
const int esperaUs = 100;          // Espera entre cada medicion en microsegundos

float factorSensor = 30.0;         // Factor del sensor (ajustable)
float valorMinLecturaRMS = 0.25;    // Valor mas bajo que se puede registrar. Para eliminar ruido.
float factorAmplificador = 10;      // Ajusta el valor según la amplificación del ccircuito de entrada

#include "externalCode/EasyElectricCalc/EasyElectricCalc.h"

// Función para dibujar una línea discontinua
void drawDashedLine(int x0, int y0, int x1, int y1, int dashLength, UI_OLED * ui) {
  bool draw = true; // Controla si dibujamos o no el segmento actual
  int deltaX = x1 - x0;
  int deltaY = y1 - y0;
  float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
  float stepX = (float)deltaX / distance;
  float stepY = (float)deltaY / distance;

  for (int i = 0; i < distance; i += dashLength) {
    if (draw) {
      for (int j = 0; j < dashLength && i + j < distance; j++) {
        int x = x0 + (i + j) * stepX;
        int y = y0 + (i + j) * stepY;
        ui->display.drawPixel(x, y, SSD1306_WHITE);
      }
    }
    draw = !draw; // Alterna entre dibujar y no dibujar
  }
  //display.display(); // Actualiza la pantalla
}


void OsciloscopeWidget(UI_OLED * ui, int pinCorriente){
  // Limpiar pantalla al iniciar
  ui->display.clearDisplay();

  bool continuarOsciloscopio = true;
  while(continuarOsciloscopio){
    // Leer el valor del ADC y escalarlo para el rango de la pantalla (0-64)
    int adcValue = analogRead(pinCorriente);
    int yPos = map(adcValue, 0, 4095, SCREEN_HEIGHT, 0); // Invertir para que 0 esté abajo

    // Guardar el valor en el array y avanzar la posición
    adcValues[xPos] = yPos;
    xPos++;

    // Si llegamos al final de la pantalla, reiniciamos
    if (xPos >= NUM_POINTS) {
      xPos = 0;

      // Calcular offset y mostrar en pantalla como una recta
      //----------------------------------------
      float voltajes[NUM_MUESTRAS];
      float offset = 0.0; // get_offset(pinCorriente);
      float valorRMS = medirValorRMS(pinCorriente,voltajes,offset);
      float corrienteRMS = calcularCorrienteRMS(valorRMS, offset);  // El offset se envia como direccion de memoria, por lo que cambia su valor automaticamente
      float posY_offset = SCREEN_HEIGHT-offset*(SCREEN_HEIGHT/3.4);
      //ui.display.drawLine(0, posY_offset, NUM_POINTS, posY_offset, SSD1306_WHITE);
      //drawDashedLine(0, posY_offset, NUM_POINTS, posY_offset, 5);
      ui->display.setTextSize(1);      // Tamaño del texto (1 es el más pequeño, se puede aumentar)
      ui->display.setTextColor(SSD1306_WHITE); // Color del texto (SSD1306_WHITE para blanco)
      ui->display.setCursor(0, 0);     // Posición del cursor en la esquina superior izquierda
      ui->display.print("Irms: ");
      ui->display.print(corrienteRMS);
      ui->display.print(" A");

      //------------------------------------------

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
        drawDashedLine(x_line, espacioSup, x_line, sizeV + espacioSup, 2, ui);
      }

      // Grilla horizontal
      for(uint8_t i=n_horizontales-1; i>0; i--){
        int y_line = sizeV - i*(sizeV/n_horizontales) + espacioSup;
        drawDashedLine(0, y_line, sizeH, y_line, 2, ui);
      }
      //------------------------------------------
      SerialBT.println(corrienteRMS);  // Enviar dato por bluetoth
      ui->display.display();
    }

    // Borrar la pantalla antes de redibujar
    ui->display.clearDisplay();

    // Dibujar la línea de osciloscopio
    for (int i = 0; i < NUM_POINTS - 1; i++) {
      int x1 = i;
      int y1 = adcValues[i];
      int x2 = i + 1;
      int y2 = adcValues[i + 1];
      if(i<=xPos) ui->display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
    }

    // Nota: Para mejorar ajuste del tiempo, se deberia leer primero todos los datos y luego mostrar en pantalla los datos acumulados

    // Mostrar el dibujo en la pantalla
    //ui.display.display();

    // Esperar un momento para evitar que se actualice demasiado rápido
    vTaskDelay(pdMS_TO_TICKS(1));

    // Verificar si el usuario quiere salir del osciloscopio
    if(ui->botonesUI.botonPresionado() == '4') continuarOsciloscopio = false;

    /*    
    double corriente = analogRead(pinCorriente)*(3.4/4096);
    Serial.print(0);
    Serial.print(" ");
    Serial.print(3.4);
    Serial.print(" ");
    Serial.println(corriente);
    vTaskDelay(pdMS_TO_TICKS(1));
    */
  }
}