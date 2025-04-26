
#ifndef EasyDataRegister_H
#define EasyDataRegister_H

// Ajustes de liberia
//-------------------------------
#define rutaSD "/sdcard/dataRegister"   // Donde se guardan los archivos
//--------------------------------


// Para el tiempo, sd y sensores. Para el serial2
//-------------------------------------
#include "..\timeFuntions.h"
#include "..\sdFun.h"
#include "..\EasySensor\EasySensor.h"

#include <HardwareSerial.h>
HardwareSerial mySerial(2);  // Usamos UART2
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

String file_type_code[] = {
    "1230",
    "1231",
    "1232",
    "1233",
    "1234",
    "1235"
};


class EasyDataRegister {

protected:
    int id = 0;              // Permite diferenciar entre varios registros del mismo tipo
    int registerFileType;    // Indica el tipo de archivo que se va a guardar (corriente, voltaje, etc)
    String currentFileName;
    bool sd_ok = true;       // Indica si la SD está lista para guardar datos
    EasySensor * sensors;    // Sensor asociado al registro
    char * oldName = "Undefined";          // Nombre del archivo anterior
public:
    EasyDataRegister(EasySensor * sensors_u, int type = undefined_file, int ID = 0):registerFileType(type),sensors(sensors_u),id(ID){
        this->sd_ok = mountSD(); // Inicializa la SD para guardar los datos. Nota: El serial quiza aun no esta disponible en este momento, por lo que no muestran mensajes
        mySerial.begin(115200, SERIAL_8N1, 16, 17);  // Configura UART2 con RX=16, TX=17
    };
    ~EasyDataRegister(){};

    // Crear archivo nuevo para comenza a guardar datos si no existe aun
    bool createNewFile(){
        //if(!getSDStatus(true)) return;  // Verificar si la SD está lista

        // Crear la carpeta del archivo
        
        if (!SD.exists("/data")) {
            createDir(SD, "/data");
        }

        // Crear el archivo
        String fileFormat = buildFileFormat();  // Construye el formato del archivo
        return writeFile(SD, "/data/rutaSD.csv", fileFormat.c_str());  // Escribe el formato del archivo     genFilePath()
    };
    
    // Funciones primordiales
    bool saveData(float * data){
        if(!getSDStatus(true)) return false;    // Verificar si la SD está lista

        // Crear archivo si no existe el de ese dia
        /*
        char * oldNameBuffer = oldName;
        if(oldNameBuffer != genFileName()){
            createNewFile();
            oldName = genFileName();
        }
        */

        if (!SD.exists("/data/rutaSD.csv")) {
            createNewFile();
        }

        String dataLine = buildFileDataLine(data);  // Construye la linea de datos a guardar en el archivo
        Serial.println(dataLine);
        File file = SD.open("/data/rutaSD.csv", FILE_APPEND);  // Abre el archivo para guardar los datos FILE_APPEND  genFilePath()
        if(!file){
            Serial.println("Failed to open file for writing");
            return false;
        }

        // Escribe los datos en el archivo:
        if(file.print(dataLine)){
            Serial.println("File written");
            file.close();
        } else {
            Serial.println("Write failed");
            file.close();
            return false;
        }
        return true;
    };
    
    bool saveDataFromSensor(){
        float * data = this->sensors->getSensorData(false); // Obtiene los datos del sensor sin hacer nueva lectura
        return saveData(data);
    };

    // Funciones de control
    bool getSDStatus(bool tryReconnect = true){  // antes era false
        if(tryReconnect) sd_ok = mountSD();  // Intenta reconectar la SD
        if (!SD.exists("/data/rutaSD.csv") && sd_ok){
            Serial.println("Error: El archivo de datos desaparecio.");

            // Volver a crear el archivo
            Serial.println("Intentando crear uno nuevo...");
            Serial.println(createNewFile());
            return false;
        }
        return sd_ok;
    };

    
    bool sendDataToSerial(float * data){
        // Enviar los datos guardados al serial
        // Nota: Esta funcion se puede usar para enviar los datos guardados a un servidor o a un dispositivo externo
        //const String data = "datosVC{17594930;2.3;4.00;3.5;4.6;3.5;6.34;60.90}"; // Ejemplo de formato
        String mensaje = "datosVC{" + String(rtc.getLocalEpoch()) + ";";
        int dataLength = sensors->getDataLength();
        for(int i=0; i<dataLength; i++){
            mensaje += String(data[i]) + ";";
        }
        mensaje += "}";

        mySerial.println(mensaje);
        Serial.print("Enviado a mySerial: ");
        Serial.println(mensaje);
        return true;
    }


    // Funciones de auxiliares para generacion de archivos
    char * genFilePath();           // Genera la ruta del archivo para guardar los datos
    char * genFileName();           // Genera un nombre de archivo para guardar los datos segun la fecha y hora
    String buildFileDescriptor(String measure_period);   // Construye el descriptor del archivo segun el tipo de archivo 
    String buildFileHeader();       // Construye el encabezado del archivo segun el tipo de archivo
    String buildFileCalcLine();     // Construye la linea de datos calculados a partir de los datos guardados
    String buildFileDataLine(float * data);  // Construye la cadena de datos a guardar en el archivo
    String buildFileSpaceLine();

    // Funciones compuestaas de auxiliares
    String buildFileFormat();         // Construye el formato del archivo para dejarlo listo para guardar datos

    /*
    // Posibles funciones futuras
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

// Generar la ruta donde se guardara el archivo
char * EasyDataRegister::genFilePath(){
    // Genera la ruta del archivo para guardar los datos
    // Se basa en la fecha y hora actual
    String filePath = String(rutaSD) + "/" + file_type[registerFileType] + "/" + String(genFileName());
    char * cstr = new char[filePath.length() + 1];
    strcpy(cstr, filePath.c_str());
    return cstr;
}

// Generar el nombre del archivo
char * EasyDataRegister::genFileName(){
    // Genera un nombre de archivo para guardar los datos
    // Se basa en la fecha y hora actual
    String fileName = rtc.getTime("%Y-%m-%d") + "@" + file_type[registerFileType] + ".csv";  //-%H-%M-%S
    char * cstr = new char[fileName.length() + 1];
    strcpy(cstr, fileName.c_str());
    this->oldName = cstr;  // Guarda el nombre del archivo para comparar luego
    return cstr;
}

// Descritor del archivo: Informacion general del archivo y de como estan ordenados los datos
String EasyDataRegister::buildFileDescriptor(String measure_period){
    // Esta funcion genera un descriptor del archivo segun el tipo de archivo
    String descriptor = file_type_code[registerFileType] + ":" + String(measure_period) + ":" + file_type[registerFileType] + ":";
    switch (registerFileType){
        case mono_current_file:
            descriptor += "mono-ampers";
            break;
        case tri_current_file:
            descriptor += "tri-ampers";
            break;
        case mono_voltage_file:
            descriptor = "mono-volts";
            break;
        case tri_voltage_file:
            descriptor += "tri-volts";
            break;
        case tri_current_voltage_file:
            descriptor += "tri-ampers & tri-volts & hertz";
            break;
        default:
            descriptor += "Archivo indefinido";
            break;
    }
    descriptor += ":" + rtc.getTime("%Y-%m-%d") + ":" + rtc.getTime("%H:%M:%S");
    return descriptor + "\n";
}

// Encabezado de los datos
String EasyDataRegister::buildFileHeader(){
    String header = "Timestamp [s];";  // Inicia con el timestamp
    switch (registerFileType){
        case mono_current_file:
            header += "Corriente [A];";
            break;
        case tri_current_file:
            header += "Corriente 1 [A];Corriente 2 [A];Corriente 3 [A];";
            break;
        case mono_voltage_file:
            header += "Voltaje [V];";
            break;
        case tri_voltage_file:
            header += "Voltaje 1 [V];Voltaje 2 [V];Voltaje 3 [V];";
            break;
        case tri_current_voltage_file:
            header += "Corriente 1 [A];Corriente 2 [A];Corriente 3 [A];Voltaje 1 [V];Voltaje 2 [V];Voltaje 3 [V];Frecuencia [Hz];";
            break;
        default:
            header += "Undefined;";
            break;
    }
    return header + "\n";
}

// Para generar lineas de datos con los valores calculados a partir de los datos guardados
String EasyDataRegister::buildFileCalcLine(){
    // Esta funcion genera una linea de datos con los valores calculados a partir de los datos guardados
    // por ejemplo, la potencia, la energia, etc.
    // Nota: Estas lineas son para guardar el analisis que se haga sobre los datos una vez terminadas las mediciones.
    //       Estan pensadas ser manipuladas por codigo externo posteriormente.
    String calcLine1 =  "updated;promedio;upTime;potenciaMax[kW];energia[kWh];\n";
    String calcLine2 =  "false;undefined;undefined;undefined;undefined;\n";
    return calcLine1 + calcLine2;
}

// Para generar lineas de datos de cada medicion
String EasyDataRegister::buildFileDataLine(float * data){
    String dataLine;
    int dataLength = sensors->getDataLength();
    dataLine += String(rtc.getLocalEpoch()) + ";";     // getExternalEpoch() para usar el tiempo externo
    for(int i=0; i<dataLength; i++){
        //Serial.println(data[i]);
        dataLine += String(data[i]) + ";";
    }
    return dataLine + "\n";
}

// Para generar lineas de espacio en el archivo
String EasyDataRegister::buildFileSpaceLine(){
    return "****************************************\n";
}

// Para generar el formato inicial completo del archivo
String EasyDataRegister::buildFileFormat(){
    // Construye el formato del archivo
    String fileFormat = buildFileDescriptor("1") +    // Descriptor del archivo
                        buildFileSpaceLine() +        // Linea de espacio
                        buildFileCalcLine() +         // Linea de calculos
                        buildFileSpaceLine() +        // Linea de espacio
                        buildFileHeader();            // Encabezado de los datos
    return fileFormat;
}


#endif // EasasyDataRegister_H

