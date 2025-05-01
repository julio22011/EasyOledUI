// EasyOledUI. Version: 0.9.5
// Esta libreria solamente funciona con la version 1.0.6 del paquete para la ESP32.
// Aun se debe identificar por que razon falla con las nuevas versiones.
//-----------------------------------------------------------------------
// Hardware usado:
//  - OLED I2C de 128x64 pixeles.
//  - Circuito de cuatro botones mediante resistencias (un ADC detecta la tecla precionada según voltaje).
//  - Transistor para manejar motor de vibracion, se activa al pricionar un boton
//-----------------------------------------------------------------------


// Configuración de pantalla OLED
//-----------------------------------------------------------------------
//#include <Adafruit_SSD1306.h>  // Ya se incluye en la libreria UI_OLED.h
#include <EasyOledUI.h> // ya incluye otras librarias requeridas

// Pines de I2C
#define I2C_SDA 22
#define I2C_SCL 21

// Definicion del tamaño de la pantalla:
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Parametros de la pantalla:
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; usually 0x3D for 128x64, 0x3C for 128x32


// Configuración de los botones y buzzer
//-----------------------------------------------------------------------
#define pinBuzzer 12     // Pin del buzzer
#define pinTransistor 33 // Pin del transistor
#define pinDeLosBotones 36   // pin con ADC donde están conectados los botones
uint8_t tipoBotonesDisp = 0; // Define el tipo de botones, 0 para fila de 4 botones, 1 para cruceta
int limitesDeLect[] = {100,4095,3900,3000,2700,2000,1600,1000,600};  // limite de no presionado, limte min y max de cada boton
            // Order: {do nothing threshold, 1Max, 1Min, 2Max, 2Min, 3Max, 3Min, 4Max, 4Min}


// Creación de las instancias
//-----------------------------------------------------------------------
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
UI_OLED ui;            // crear objeto de UI
menu menuPrincipal;    // Menu inicial de la UI
menu menuSecundario;   // Menu secuandario de la UI
botones botones;       // Botones que manejan la UI

void setup() {

  Serial.begin(115200); // inicia el puerto serial a 115200 baudios

  // Definir comportamiento de pines
  //?????????????????????????????????????????????????????????????????????????????????//
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinTransistor, OUTPUT);

  // Dejar apagado el motor de vibracion conectado al transistor (durante el arranque)
  //?????????????????????????????????????????????????????????????????????????????????//
  digitalWrite(pinTransistor, LOW);

  // Sonido de arranque mediante buzzer
  //?????????????????????????????????????????????????????????????????????????????????//
  digitalWrite(pinBuzzer, HIGH);
  vTaskDelay(pdMS_TO_TICKS(10));
  digitalWrite(pinBuzzer, LOW);

  // Poner a funcionar la UI
  //?????????????????????????????????????????????????????????????????????????????????//
  crearUI();         // Funcion que inicia la la pantalla y crea la UI
  iniciarUI(3000);   // Funcion que inica agrega la ejecución de la UI al multitasking nativo del ESP32

  inicializarAcciones();
}


bool callBackEjecutarAccionExterna(int menuActual, int opcionActual){}


void loop() {
  // put your main code here, to run repeatedly:
}


// Crea la ui que se mustra en la pantalla
void crearUI(){
  // Asociar la pantalla a la UI y inicializar:
  //---------------------------------------------------------------------------------------
  //Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // Cambio: Se traslado a variable global
  ui.asociarPantalla(&display);            // Asociar la pantalla al UI
  ui.setupPantallaOled(I2C_SDA, I2C_SCL); // Iniciar la ui junto con la pantalla: Se indican los pines de I2C
  // Nota: 
  //      Si la pantalla es por SPI, se debe cambiar ui.setupPantallaOled() por el proceso de inicio correspondiente. 
  //      Por el momento, ui.setupPantallaOled() solamente soporta I2C.

  // Creacion del menu principal
  // ---------------------------------------------------------------------------------------
  // Definir la opciones mostradas en la pantalla:
  String titulosM0[] = {
    "Musica",
    "Patron de vibracion",
    "Animacion",
    "Invertir colores",
    "Opcion 04",
    "Opcion 05"};

  // Definir las funciones que ejecutan en cada opcion del menu O
  FuncionUI accionesM0[] = {
    AccM00,
    AccM01,
    AccM02,
    AccM03,
    NULL,
    NULL,   // Dejar en NULL si no se desea hacer nada: Indica error O1. Poner funcion vacia si se quiere eliminar el error.
  };

  // Agregar el menu creado
  int numDeOpciones = (sizeof(titulosM0)/sizeof(titulosM0[0]));                           // Determinar la cantidad de elementos de este menu
  menuPrincipal.configurarMenu(0, numDeOpciones, "Menu inicial", titulosM0, accionesM0);  // Configurar el menu: (numero de menu, cantidad de opciones, titulo, opciones, opcion inicial)
  //menuPrincipal.imprimirOpcionesEnSerial();                                             // Para depuracion: Imprime por serial la opciones del menu

  // Creacion de un menu secundario
  // ---------------------------------------------------------------------------------------
  // Definir la opciones mostradas en la pantalla:
  String titulosMenuSecundario[] = {
    "Sonido 10",
    "Sonido 11",
    "Sonido 12",
    "Sonido 13",
    "Sonido 14"};
  
  // Definir las funciones que ejecutan en cada opcion del menu 1
  FuncionUI accionesM1[] = {
    AccM10,
    AccM11,
    AccM12,
    AccM13,
    AccM14,
  };

  // Agregar el menu creado
  int numDeOpcionesSec = (sizeof(titulosMenuSecundario)/sizeof(titulosMenuSecundario[0])); // determinar la cantidad de elementos
  menuSecundario.configurarMenu(1, numDeOpcionesSec, "Musica", titulosMenuSecundario, accionesM1);
  //menuSecundario.imprimirOpcionesEnSerial();  // funcion de depuracion

  // Asociar los menus creados a la ui
  // ---------------------------------------------------------------------------------------
  menu todosLosMenus[] = {menuPrincipal,menuSecundario}; // Vector de los menus
  ui.asociarMenu(2, todosLosMenus);
  //ui.imprimirTitulosDeMenusSerial(); // funcion de depuracion

  // Configurar botones:
  // ---------------------------------------------------------------------------------------
  botones.iniciar(pinDeLosBotones, tipoBotonesDisp, limitesDeLect, pinTransistor); // int pin = 36, uint8_t tipo = 0, int *limitesDeLectura = NULL
  ui.asociarBotones(&botones);
  //attachInterrupt(digitalPinToInterrupt(ui.botonesUI.pinBotones), handleInterrupt, CHANGE); // agregar una rutina de interrupcion a los botones. No funciono

  // Mostrar mensaje de bienvenida
  // ---------------------------------------------------------------------------------------
  ui.mostrarMensaje("Bienvenido");        // Prueba de enviar un mensaje
  vTaskDelay(pdMS_TO_TICKS(1000));        // Esperar para no borrar el mensaje y poder verlo
}



// Funciones de las acciones
//====================================================

// Menu principal M0
void AccM00(){
  ui.menuActual = 1;      // Cambiar a otro menu 
}

void AccM01(){
  Serial.println("Activando vibracion");
  ui.mostrarMensaje("Vibrando");  // Mostrar un mensaje temporalmente

  // Patron de vibracion
  for(int i;i<5;i++){
    digitalWrite(pinTransistor, HIGH);
    vTaskDelay(pdMS_TO_TICKS(50));
    digitalWrite(pinTransistor, LOW);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  for(int i;i<3;i++){
    digitalWrite(pinTransistor, HIGH);
    vTaskDelay(pdMS_TO_TICKS(70));
    digitalWrite(pinTransistor, LOW);
    vTaskDelay(pdMS_TO_TICKS(60));
  }
  for(int i;i<5;i++){
    digitalWrite(pinTransistor, HIGH);
    vTaskDelay(pdMS_TO_TICKS(50));
    digitalWrite(pinTransistor, LOW);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void AccM02(){
  // Ejecutar test de lineas
  testdrawrect();
}

void testdrawrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  //vTaskDelay(pdMS_TO_TICKS(1000));
}

bool estadoInvversionPantalla = false;
void AccM03(){
  estadoInvversionPantalla = !estadoInvversionPantalla;
  display.invertDisplay(estadoInvversionPantalla);
}

// Menu secundario M1
void AccM10(){
  // Cancion 1 con el buzzer
  for(int i ;i<10; i++){
    digitalWrite(pinBuzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    digitalWrite(pinBuzzer, LOW);
    vTaskDelay(pdMS_TO_TICKS(100));
    digitalWrite(pinBuzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(20));
    digitalWrite(pinBuzzer, LOW);
  }
}

void AccM11(){
  // Cancion 2 con el buzzer
  for(int i ;i<10; i++){
    digitalWrite(pinBuzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    digitalWrite(pinBuzzer, LOW);
    vTaskDelay(pdMS_TO_TICKS(100));
    digitalWrite(pinBuzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(20));
    digitalWrite(pinBuzzer, LOW);
  }
}

void AccM12(){
  // Cancion 3 con el buzzer
  for(int i ;i<10; i++){
    digitalWrite(pinBuzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    digitalWrite(pinBuzzer, LOW);
    vTaskDelay(pdMS_TO_TICKS(50));
    digitalWrite(pinBuzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(30));
    digitalWrite(pinBuzzer, LOW);
  }
}

void AccM13(){
  // Cancion 4 con el buzzer
  for(int i ;i<10; i++){
    digitalWrite(pinBuzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    digitalWrite(pinBuzzer, LOW);
    vTaskDelay(pdMS_TO_TICKS(100));
    digitalWrite(pinBuzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(20));
    digitalWrite(pinBuzzer, LOW);
  }
}

void AccM14(){
  // Cancion 5 con el buzzer
  for(int i ;i<10; i++){
    digitalWrite(pinBuzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    digitalWrite(pinBuzzer, LOW);
    vTaskDelay(pdMS_TO_TICKS(200));
    digitalWrite(pinBuzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(20));
    digitalWrite(pinBuzzer, LOW);
  }
}
//====================================================





// Configuracion del multitasking
//---------------------------------------------------------------------

// Definicion de la terea que actualiza la UI constantemenete
void IRAM_ATTR tareaUI(void * parameters){
  // Tarea que mantiene funcionando el UI de la OLED
  while(true){    
    ui.update();
    vTaskDelay(pdMS_TO_TICKS(20));  // Delay para no sobrecargar tarea
  }
  vTaskDelete(NULL);
}

// Configuracion de la tarea que ejecuta la UI
//TaskHandle_t TaskHandle_1;
void iniciarUI(int stack) {    //  stack> 1000
  BaseType_t tar1 = xTaskCreatePinnedToCore(
    tareaUI,            // Funcion que se llama se llama en la tarea
    "UI-OLED",          // Nombre de la tarea (para depuracion)
    stack,              // Tamaño del stack em bytes
    NULL,               // Parameter to pass
    1,                  // Prioridad de la tarea
    NULL,               // Task handle, NULL si no tiene
    1                   // Nucleo en el que corre
  );
}