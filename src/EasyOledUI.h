
/* En este archivo se encuentra la calse que permite
 * crear una interfaz de usuario en la pantalla OLED
*/

#if !defined(librericaPreferenciasIncluida)
#define librericaPreferenciasIncluida
#include <Preferences.h>                  // incluir libreria que permite guardar Ajustes en la memoria
#endif



#if !defined(claseUI_OLEDIncluida) // if para evitar ejecutar el mismo codigo dos veces
#define claseUI_OLEDIncluida

//=====================================================================
//           Parametros generales de la pantalla
//=====================================================================

// Definicion del tamaño de la pantalla:
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Parametros de la pantalla:
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define duracionMensajeMS 1000 // Tiempo de espera para mostrar mensajes de error en milisegundos
//=====================================================================

#include <Adafruit_SSD1306.h>   // se requiere la clase de la pantalla
#include <Wire.h>

// Otras clases asociadas
#include "claseMenu.h"          // se requiere la clase menu para que esta clase funcione
#include "claseBotones.h"       // se requiere esta clase para enlazar los botones con la ui
//#include "claseSensor.h"      // Se requiere para mostrar lectruas de los sensores

/*
// Funcion que puede ser modificada desde afuera. Permite hacer acciones disparadas desde el menu
bool callBackEjecutarAccionExterna(int menuActual, int opcionActual);

bool callBackEjecutarAccion(int menuActual, int opcionActual){
  callBackEjecutarAccionExterna(menuActual, opcionActual);
}
*/


class UI_OLED {
  //
  private:

  public:
  
  // Varibles
  Adafruit_SSD1306 display; // permite almacenar la clase de la pantalla asociada
  uint16_t menuActual = 0;
  int numeroMenus = 0;                      // guarda la cantidad de menus existentes
  menu *menusGuardados = NULL;              // Puntero hacia los objetos de tipo menu
  botones botonesUI;
  bool cambioPendiente = true;              // Permite indicar desde el exterior de la clase si se debe refrescar los menus
  bool mensajePendiente = false;            // Permite indicar desde el exterior si de debe mostrar un mensaje
  String mensajeEsporadico = "None";

  // Funciones
  bool mostrarMenuEnOLED(menu menuPorMostrar);
  bool asociarPantalla(Adafruit_SSD1306 displayPorAsociar);
  bool asociarBotones(botones _botones);
  void handleInterruptUI();
  bool asociarMenu(int numMenus, menu *menusPorAsociar);
  bool imprimirTitulosDeMenusSerial();
  bool setupPantallaOled(int I2C_SDA, int I2C_SCL);
  //bool mostrarLecturasSensores(sensor *sensores, int cantSensores);
  //bool mostrarVoltajesSensores(sensor *sensores, int cantSensores);
  bool mostrarMensaje(String mensaje, int duracion);
  bool callBackEjecutarAccionMenu(int menuActual, int opcionActual);
  bool update();

  void testdrawbitmap(void);
};


// Funciones de la clase
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool UI_OLED::asociarMenu(int numMenus, menu *menusPorAsociar){
  // Permite agregar los menus al objeto
  numeroMenus = numMenus;
  menusGuardados = new menu[numMenus];
  for (int i = 0; i < numMenus; i++)
  {
    menusGuardados[i] = menusPorAsociar[i]; // asocia cada uno de los menus
  }
  return 1;
}

bool UI_OLED::asociarPantalla(Adafruit_SSD1306 displayPorAsociar){
  // Esta funcion vincula el UI con la pantalla OLED
  display = displayPorAsociar;
}

bool UI_OLED::asociarBotones(botones _botones){
  // Esta funcion vincula los botones con la UI
  botonesUI = _botones;
}

// Funcion ISR para manejar las interrupciones de los botones
void UI_OLED::handleInterruptUI() {
  // Código a ejecutar cuando se produce la interrupción
  Serial.println("ISR: Boton presionado.");
}

bool UI_OLED::imprimirTitulosDeMenusSerial(){
  for (int i = 0; i < numeroMenus; i++){
    Serial.println(menusGuardados[i].titulo);
  }
  return 1;
}

// Funcion para iniciar la pantalla
bool UI_OLED::setupPantallaOled(int I2C_SDA, int I2C_SCL){
  Wire.begin(I2C_SDA, I2C_SCL);  // iniciar I2C con los pines definidos
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  //display.display();
  testdrawbitmap(); // Muestra un logo en la pantalla
  vTaskDelay(pdMS_TO_TICKS(4000)); // Pause for 4 seconds
  display.clearDisplay();
  
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
}

// Logo inicial
// --------------------------------------
#include "logoInicial.h"  // Matriz con la imagen del logo y el tamaño
void UI_OLED::testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    LogoWaveSense, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
}
// --------------------------------------

bool UI_OLED::mostrarMenuEnOLED(menu menuPorMostrar){
  // Funcion para desplagar un menu en la pantalla
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.drawLine(1, 15, display.width()-15, 15, SSD1306_WHITE);
  display.setCursor(0,6);
  display.println(menuPorMostrar.titulo); // mostrar el titulo
  display.setCursor(0,17);
  for (int i = 0; i < menuPorMostrar.numOpc; i++)
  {
    if(i==menuPorMostrar.opcionSelecionada){ // verifica cual es la opcion en pantalla y le pone una marca
      display.println("> " + menuPorMostrar.opciones[i]);
    } else{
      display.println("  " + menuPorMostrar.opciones[i]);
    }
  }
  display.display();
}


// Funcion para mostrar un mensaje de aviso
bool UI_OLED::mostrarMensaje(String mensaje, int duracion = duracionMensajeMS){
  display.clearDisplay();             // Borra el buffer de la pantalla
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(5,32);

  display.println(mensaje);  // probando si devuelve valor
  display.display();
  vTaskDelay(pdMS_TO_TICKS(duracion)); // Pausa para mostrar el mensaje
}

// Ejecuta la accion que especifico el menu
bool UI_OLED::callBackEjecutarAccionMenu(int menuActual, int opcionActual){
  if(menusGuardados[menuActual].accionesMenu != NULL){  // Verficar si el vector no esta Null para evitar error
    if(menusGuardados[menuActual].accionesMenu[opcionActual] != NULL){
      menusGuardados[menuActual].accionesMenu[opcionActual]();  // Ejecutar la accion asociada a la opcion seleccionada
      return true;
    }
  }
  mostrarMensaje("Error O1");
  Serial.println("Error O1: No hay accion asociada a la opcion seleccionada.");
  return false;
}

// if algo cambia, actualizar pantalla
bool IRAM_ATTR UI_OLED::update(){
  char botonPresionado = botonesUI.botonPresionado();  // Varible para recibir el boton presionado
  if (botonPresionado != '0'){ // si se presiona un boton, verifica cual fue
    if(botonPresionado == '1'){
      menusGuardados[menuActual].dismimuirOpcionSelecionada(1);
    } else if (botonPresionado == '2'){
      menusGuardados[menuActual].aumentarOpcionSelecionada(1);
    }
    if(botonPresionado == '3'){
      //selectorDeAccion();
      // permite ejecutar una accion desde el exterior segun el menu y la opcion seleccionada
      //callBackEjecutarAccion(menuActual, menusGuardados[menuActual].opcionSelecionada); // Ya no se necesita
      callBackEjecutarAccionMenu(menuActual, menusGuardados[menuActual].opcionSelecionada);
    }
    if(botonPresionado == '4'){
      //cambiarDeMenu();
      if(menuActual != 0) menuActual--;
    }
    mostrarMenuEnOLED(menusGuardados[menuActual]); // actualiza el menu
  }

  if(mensajePendiente){
    // Permite activar mensaje esporadico
    mostrarMensaje(mensajeEsporadico);
    mensajePendiente = false;    // Elimina el pendiente
  }
  if(cambioPendiente){
    // Actualiza pantalla si se hace alguna modificacion afuera que requiera un refresco
    mostrarMenuEnOLED(menusGuardados[menuActual]); // actualiza el menu
    cambioPendiente = false;
  }
  return 1;
} // fin de funcion


/*
bool UI_OLED::mostrarLecturasSensores(sensor *sensores, int cantSensores){ // String *nombreSensores, float * lecturas
  // Muestra las lecturas de los sensores 
  display.clearDisplay();             // Borra el buffer de la pantalla
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,8);

  //int cantSensores = sizeof(sensores)/sizeof(sensor); // Determina cuantos sensores son
  display.println("Lecturas");
  display.println("------------");
  for(int i = 0; i < 5; i++){  //cantSensores
    display.print(sensores[i].parametro + ": ");
    display.print(sensores[i].devolverParametroFisico());  // Devulve el valor
    display.println(" " + sensores[i].devolverUnidadesFisicas());
  }
  display.display();
  
  // Si son mas de 4 sensores:
  if (cantSensores > 5){
    vTaskDelay(pdMS_TO_TICKS(1000));
    display.clearDisplay();             // Borra el buffer de la pantalla
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,8);
    display.println("Lecturas 2");
    display.println("------------");
    for(int i = 5; i < cantSensores; i++){
      display.print(sensores[i].parametro + ": ");
      display.print(sensores[i].devolverParametroFisico());  // Devulve el valor
      display.println(" " + sensores[i].devolverUnidadesFisicas());
    }
    display.display();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

bool UI_OLED::mostrarVoltajesSensores(sensor *sensores, int cantSensores){ // String *nombreSensores, float * lecturas
  // Muestra las lecturas de los sensores 
  display.clearDisplay();             // Borra el buffer de la pantalla
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,8);

  //int cantSensores = sizeof(sensores)/sizeof(sensor); // Determina cuantos sensores son
  display.println("Voltajes");
  display.println("------------");
  for(int i = 0; i < cantSensores; i++){
    display.print(sensores[i].parametro + ": ");
    display.print(sensores[i].voltaje);  // Devulve el valor
    display.println(" volts");
  }
  display.display();
}
*/


#endif
