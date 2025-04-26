

#ifndef EasySensor_H
#define EasySensor_H

//#include "..\EasyElectricCalc\EasyElectricCalc.h"  // Funciones para cálculos eléctricos

enum {
    undefined_sensor,
    mono_current_sensor,
    tri_current_sensor,
    mono_voltage_sensor,
    tri_voltage_sensor,
    tri_current_voltage_sensor
};

const int type_dataLength[] = {1, 2, 4, 2, 4, 7};   // Longitud de los datos de cada sensor


class EasySensor {
protected:
    int sensorType; // Indica el tipo de sensor
    bool sensorStatus = false;  // Indica si el sensor está listo para leer
    float * sensorData;         // Valor del sensor: Al ser un puntero, se puede cambiar el tamaño del array
    int dataLength = -1;         // Longitud de los datos del sensor
public:
    EasySensor(int sensorType_u = undefined_sensor) : sensorType(sensorType_u){};
    ~EasySensor(){};

    // Obtener tamaño de los datos
    int getDataLength(){
        if(sensorData != NULL){
            dataLength = type_dataLength[this->sensorType];   // Obtener la longitud de los datos
        } else dataLength = -1;
        return dataLength;
    };

    // Funciones
    bool getSensorStatus(){return sensorStatus;};

    float * getSensorData(bool newRead = true){
        if(getSensorStatus()){
            if(!newRead){
                if(sensorData != NULL) return sensorData;  // Devolver los valores actuales sin hacer nueva lectura
                else{
                    Serial.println("Error s5005: Sensor data not defined."); 
                    static float errorData[] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0};  // Devolver un valor de error
                    return errorData;
                }
            } 
            if(read()) return sensorData;  // // Realizar proceso de lectura y Devolver los valores
            else{Serial.println("Error s: Read fun not defined on sensor."); return NULL;}
        }

        // En caso de que el sengun el sensor no esté listo:
        Serial.println("Error S1: Sensor not ready.");
        return NULL;
    };

    // Set de valor resultante
    void setSensorData(float * data_u){
        sensorData = data_u;
    };

    // Funciones virtuales: Pueden cambiar segun el sensor
    virtual bool begin(){Serial.println("Error S2: Begin fun not defined on sensor."); return false;}; // Inicializa el sensor
    virtual bool read(){Serial.println("Error S3: Read fun not defined on sensor."); return false;};  // Guarda el valor en sensorData
    virtual float * getSensorChanelRawData(int chanel){Serial.println("Error S4: getSensorChanelRawData fun not defined on sensor."); return NULL;};  // Devuelve los valores de los canales
    virtual unsigned long * getSensorChanelRawTimes(int chanel){Serial.println("Error S5: getSensorChanelRawTimes fun not defined on sensor."); return NULL;};  // Devuelve los tiempos de los canales
    virtual float getSensorChanelOffset(int chanel){Serial.println("Error S6: getSensorChanelOffset fun not defined on sensor."); return -1;};  // Devuelve el offset del canal
};

// Archivos de clases secundarias
#include "ES_tri_current_voltage.h"


#endif // EasySensor_H

    



