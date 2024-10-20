//#include "esp32-hal-gpio.h"

/* En este archivo se crea la clase que permitira manejar los 4 botones
 * del dispositivo.
*/


//---------------------------------------------------------------------
// Ajustes:
#define duracionVibracion 100 // Duracion de la vibracion en milisegundos
//#define depuracionBotones // Permite depurar los botones

//---------------------------------------------------------------------

#if !defined(librericaPreferenciasIncluida)
#define librericaPreferenciasIncluida
#include <Preferences.h>                  // incluir libreria que permite guardar Ajustes en la memoria
#endif


#if !defined(claseBotonesIncluida) // if para evitar ejecutar el mismo codigo dos veces

#define claseBotonesIncluida

class botones{
  //
  private:
  uint8_t tipoBotones; // para indicar el tipo de botones, 0 para tipo 1,2,3,4 y  1 para derecha, abajo, arriba, izquierda

  public:
  // Variables
  //*************************
  int lecturaBotones;
  int lmiteNoBoton;
  int pinBotones;
  int pinMotorVibracion;
  bool activarVibracion = false;

  // Boton derecha:
  int limiteSupDer; int limiteInfDer;
  // Boton abajo:
  int limiteSupAba; int limiteInfAba;
  // Boton arriba:
  int limiteSupArr; int limiteInfArr;
  // Boton izquierda:
  int limiteSupIzq; int limiteInfIzq;

  // Funciones
  //*************************
  bool iniciar(int pin, uint8_t tipo, int *limitesDeLectura, int pinMotorVib);
  char botonPresionado();
};

// Ingresa a la configuracion los limites de lectura
bool botones::iniciar(int pin = 36, uint8_t tipo = 0, int *limitesDeLectura = NULL, int pinMotorVib = NULL){  // limites: no boton, e inferior y superior de 1,2,3 y4

  pinMode(pin, INPUT); // poner el pin como entrada

  pinBotones = pin;   // Indica pin donde estan conectados los botones
  tipoBotones = tipo; // Configura los botoenes segun su tipo                  // = {100,3900,4095,2700,3000,1600,2000,600,1000}

  // Cuando no esta presionado ningun boton:
  lmiteNoBoton = limitesDeLectura[0];
  
  // Boton derecha:
  limiteSupDer = limitesDeLectura[1];
  limiteInfDer = limitesDeLectura[2];
  // Boton abajo:
  limiteSupAba = limitesDeLectura[3];
  limiteInfAba = limitesDeLectura[4];
  // Boton arriba:
  limiteSupArr = limitesDeLectura[5];
  limiteInfArr = limitesDeLectura[6];
  // Boton izquierda:
  limiteSupIzq = limitesDeLectura[7];
  limiteInfIzq = limitesDeLectura[8];

  if(pinMotorVib != NULL){
    pinMotorVibracion = pinMotorVib;
    pinMode(pinMotorVibracion, OUTPUT);
    activarVibracion = true;
  }

  return true;
}

// Funcion para detectar el boton presionado
char botones::botonPresionado(){
  
  lecturaBotones = analogRead(pinBotones);   // Almacena la lectura analogica de los botones
  char botonPress;                           // Almacena el boton presionado

  if (lecturaBotones > lmiteNoBoton){
    if (tipoBotones == 0){
      if (lecturaBotones>=limiteInfDer && lecturaBotones<=limiteSupDer)       {botonPress = '1';}
      else if (lecturaBotones>=limiteInfAba && lecturaBotones<=limiteSupAba)  {botonPress = '2';}
      else if (lecturaBotones>=limiteInfArr && lecturaBotones<=limiteSupArr)  {botonPress = '3';}
      else if (lecturaBotones>=limiteInfIzq && lecturaBotones<=limiteSupIzq)  {botonPress = '4';}
    } else if (tipoBotones == 1) {
      if (lecturaBotones>limiteInfDer && lecturaBotones<limiteSupDer)        {botonPress = 'd';}
      else if (lecturaBotones>limiteInfAba && lecturaBotones<=limiteSupAba)  {botonPress = 'a';}
      else if (lecturaBotones>limiteInfArr && lecturaBotones<=limiteSupArr)  {botonPress = 'A';}
      else if (lecturaBotones>limiteInfIzq && lecturaBotones<=limiteSupIzq)  {botonPress = 'i';}
    }

    #if defined(depuracionBotones)
    Serial.print("Boton presionado: "); Serial.print(botonPress); Serial.print(" ");
    Serial.println(lecturaBotones); // para depuracion
    #endif

    // Ejecutar vibracion si esta activada:
    if(activarVibracion) {                      
      digitalWrite(pinMotorVibracion, HIGH);
      vTaskDelay(pdMS_TO_TICKS(duracionVibracion));
      digitalWrite(pinMotorVibracion, LOW);
    }

    return botonPress;

  } else{
    return '0'; // retorna 0 si no se presiona nignun boton
  }
}

#endif // finaliza el if

