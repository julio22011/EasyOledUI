
#ifndef sdFun_H
#define sdFun_H

// Para la memoria SD
//-----------------------------------------------------------------------
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Ajustes
#define SD_CS 4

// Variables de estado
bool errorSD = false;
bool SD_init = false;


// Funciones para la memoria SD
//---------------------------------------------------------------------
bool mountSD(){
    if(!SD.begin(SD_CS)){ // Usando 4 como CS
        Serial.println("Card Mount Failed");
        errorSD = true;
        return false;
    }

    if(SD_init) return true; // No repetir el proceso cada vez que se consulta si sigue conectata la SD
    SD_init = true;  // Indica que la SD ya fue inicializada para no repetir el proceso

     // Estado de tarjeta SD
    //?????????????????????????????????????????????????????????????????????????????????//
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    return true; // Muestra que la SD está lista
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        //ui.mostrarMensaje("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        //ui.mostrarMensaje("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            //ui.mostrarMensaje(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            //ui.mostrarMensaje(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
        //ui.mostrarMensaje("Dir created");
    } else {
        Serial.println("mkdir failed");
        //ui.mostrarMensaje("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
        //ui.mostrarMensaje("Dir removed");
    } else {
        Serial.println("rmdir failed");
        //ui.mostrarMensaje("rmdir failed");
    }
}


// Funcion para escribir en un archivo
bool writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        //ui.mostrarMensaje("Failed to open file for writing");
        return false;
    }
    if(file.print(message)){
        Serial.println("File written");
        //ui.mostrarMensaje("File written");
    } else {
        Serial.println("Write failed");
        //ui.mostrarMensaje("Write failed");
        return false;
    }
    file.close();
    return true;
}



// Función para crear una estructura de carpetas
bool createDirectories(const char *path) {
  String fullPath = "";
  String directory = "";
  bool success = true;

  // Divide la ruta en partes usando '/'
  for (int i = 0; path[i] != '\0'; i++) {
    char c = path[i];
    if (c == '/' || path[i + 1] == '\0') { 
      if (path[i + 1] == '\0') directory += c; // Añade el último segmento

      fullPath += directory; // Construye el camino hasta el segmento actual
      if (!SD.exists(fullPath.c_str())) {  // Verifica si el directorio existe
        if (!SD.mkdir(fullPath.c_str())) { // Intenta crearlo si no existe
          Serial.print("Error al crear: ");
          Serial.println(fullPath);
          success = false;
        }
      }
      directory = ""; // Reinicia para el siguiente segmento
    } else {
      directory += c;
    }
  }

  return success;
}




#endif //sdFun_H
