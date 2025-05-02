
#include <Arduino.h> // For String
#include <Adafruit_GFX.h> // For Adafruit graphics functions
#include <Adafruit_SSD1306.h> // For Adafruit SSD1306 and SSD1306_WHITE

// En este archivo se eencuentran funciones especiales para dibujar en la pantalla OLED
// --------------------------------------------------------------------------------------------


// Función para dibujar una línea discontinua
void drawDashedLine2(int x0, int y0, int x1, int y1, int dashLength, Adafruit_SSD1306 * display) {
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
        display->drawPixel(x, y, SSD1306_WHITE);
      }
    }
    draw = !draw; // Alterna entre dibujar y no dibujar
  }
  //display.display(); // Actualiza la pantalla
}


void printUnderlined(const String& text, int x, int y, int textSize, Adafruit_SSD1306 * display) {
  display->setTextSize(textSize);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(x, y);
  display->print(text);

  int charWidth = 2 * textSize; // ancho aproximado por caracter
  int textWidth = text.length() * charWidth;
  int underlineY = y + (8 * textSize); // altura de la línea base

  display->drawLine(x, underlineY, x + textWidth, underlineY, SSD1306_WHITE);
}
