
#ifndef EasyDataRegister_H
#define EasyDataRegister_H

// Ajustes de liberia
//-------------------------------
#define rutaSD "/sdcard"   // Donde se guardan los archivos
//--------------------------------


// Para el tiempo, sd y sensores
//-------------------------------------
#include "..\timeFuntions.h"
#include "..\sdFun.h"
#include "..\EasySensor\EasySensor.h"
//-------------------------------------


enum {
    undefined_file,
    mono_current_file,
    tri_current_file,
    mono_voltage_file,
    tri_voltage_file,
    tri_current_voltage_file
};

String file_type[] = {
    "undefined_file",
    "mono_current_file",
    "tri_current_file",
    "mono_voltage_file",
    "tri_voltage_file",
    "tri_current_voltage_file"
};

const uint8_t file_type_code[] = {
    1230,
    1231,
    1232,
    1233,
    1234,
    1235
};


class DataRegister {

protected:
    int id = 0;              // Permite diferenciar entre varios registros del mismo tipo
    int registerFileType;    // Indica el tipo de archivo que se va a guardar (corriente, voltaje, etc)
    String currentFileName;
    bool sd_ok = true;      // Indica si la SD está lista para guardar datos
public:
    DataRegister(int type = undefined_file, int ID = 0):registerFileType(type),id(ID) {
        sd_ok = initSD(); // Inicializa la SD para guardar los datos. Nota: El serial quiza aun no esta disponible en este momento, por lo que no muestran mensajes
    };
    ~DataRegister(){};

    bool saveData(float * data);
    bool getSDStatus(bool tryReconnect = false){
        if(tryReconnect) sd_ok = initSD();  // Intenta reconectar la SD
        return sd_ok;
    };


    // Funciones de auxiliares
    char * genFileName();  // 
    String buildFileHeader();  // Construye el encabezado del archivo segun el tipo de archivo
    String buildFileDataLine(float * data);  // Construye la cadena de datos a guardar en el archivo

    /*
    void setRegisterData(const std::string& data);
    void setRegisterData(const std::string& data, const std::string& key);
    std::string getRegisterData();
    std::string getRegisterData(const std::string& key);
    void clearRegisterData();
    void clearRegisterData(const std::string& key);
    void clearAllRegisterData();
    void clearAllRegisterData(const std::string& key);
    void setRegisterData(const std::string& data, const std::string& key, const std::string& path);
    std::string getRegisterData(const std::string& key, const std::string& path);
    void clearRegisterData(const std::string& key, const std::string& path);
    void clearAllRegisterData(const std::string& key, const std::string& path);
    void setRegisterData(const std::string& data, const std::string& key, const std::string& path, const std::string& subKey);
    std::string getRegisterData(const std::string& key, const std::string& path, const std::string& subKey);
    void clearRegisterData(const std::string& key, const std::string& path, const std::string& subKey);
    void clearAllRegisterData(const std::string& key, const std::string& path, const std::string& subKey);
    void setRegisterData(const std::string& data, const std::string& key, const std::string& path, const std::string& subKey, const std::string& subPath);
    std::string getRegisterData(const std::string& key, const std::string& path, const std::string& subKey, const std::string& subPath);
    void clearRegisterData(const std::string& key, const std::string& path, const std::string& subKey, const std::string& subPath);
    void clearAllRegisterData(const std::string& key, const std::string& path, const std::string& subKey, const std::string& subPath);
    void setRegisterData(const std::string& data, const std::string& key, const std::string& path, const std::string& subKey, const std::string& subPath
    */
};

char * DataRegister::genFileName(){
    // Genera un nombre de archivo para guardar los datos
    // Se basa en la fecha y hora actual
    String fileName = rtc.getTime("%Y-%m-%d") + "@" + file_type[registerFileType] + ".csv";  //-%H-%M-%S
    char * cstr = new char[fileName.length() + 1];
    strcpy(cstr, fileName.c_str());
    return cstr;
}

String DataRegister::buildFileDataLine(float * data){
    String dataLine;
    for(int i=0; i<sizeof(data); i++){
        //Serial.println(data[i]);
        dataLine += String(data[i]) + ";";
    }
    return dataLine + "\n";
}

#endif // EasasyDataRegister_H

