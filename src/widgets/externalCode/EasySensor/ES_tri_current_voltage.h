

#ifndef ES_tri_current_H
#define ES_tri_current_H

#include "EasySensor.h"  // Clase base para los sensores
#include "ES_chanel.h"   // Objeto para designar cada canal de sensor

// Ajustes de la librería
#define adcVoltage 3.4
#define adcResolution 4096
#define bufferSize 200         // tamano del buffer de lecturas y de tiempos (probado con 500, muy lento)
#define deltaMS 800

#define factorSensorCorriente 30.0
#define factorAmplifidorCorriente 10.0   // 10

#define factorSensorVoltaje 1.0
#define factorAmplifidorVoltaje 10.0

#define valorMinLecturaRMS_VI 0.01

// Para medir el tiempo
/*
hw_timer_t *timer = NULL;
bool timerStarted_j = false;
*/

// Posible solucion: usar timer con funcion innterrpt que mida cada 100 us

//==================================================================
bool multiMeasureRMS(ES_chanel * chanels, const int n_chanels, int NUM_MUESTRAS = bufferSize, int esperaUs = deltaMS){
    unsigned long tiempoInicial = micros();
    
    /* Para usar timer en lugar de micros
    if(!timerStarted_j){
        timer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1 MHz)
        timerStarted_j = true;
    }
    */

    // Paso 1: Leer y guardar voltajes durante un tiempo específico (muestreo)
    int i = 0;
    while (i < NUM_MUESTRAS) {
        //unsigned long tiempoMarcado = timerRead(timer);
        for(int j = 0; j < n_chanels; j++){
            chanels[j].setReadTime(micros()-tiempoInicial, i);  // sin timer
            //chanels[j].setReadTime(tiempoMarcado, i);  // usando timer de esp32
            chanels[j].setReadVoltage(analogRead(chanels[j].getSensorPin()) * (adcVoltage / adcResolution), i);
        }
        i++;
        //timerRestart(timer);   // reiniciar timer de esp32
        //delayMicroseconds(esperaUs);
        ets_delay_us(esperaUs);
    }
    unsigned long total_measument_time = micros() - tiempoInicial;
    for(int j = 0; j < n_chanels; j++) chanels[j].setLastReadTime(total_measument_time); // Actualizar el tiempo de la última lectura

    // Paso 2: Calcular el offset (promedio de los voltajes leídos)
    for(int j = 0; j < n_chanels; j++){
        float offset = 0;
        for (int k = 0; k < i; k++) {
            offset += chanels[j].getReadVoltage(k);
        }
        chanels[j].setOffset(offset / NUM_MUESTRAS); // Offset promedio
    }

    // Paso 3: Calcular el RMS sin el offset
    for(int j = 0; j < n_chanels; j++){
        float sumatoriaCuadrados = 0;
        for (int k = 0; k < i; k++) {
            float voltajeCorregido = chanels[j].getReadVoltage(k) - chanels[j].getOffset();
            sumatoriaCuadrados += sq(voltajeCorregido);
        }
        float valorRMS = sqrt(sumatoriaCuadrados / NUM_MUESTRAS); // Calcular RMS

        if(valorRMS<chanels[j].getValorMinLecturaRMS()) chanels[j].setRMSValue(0.0); // Filtro de valores muy bajos
        else chanels[j].setRMSValue(valorRMS);
    }
    
    /*
    // Contruir valores retornados
    float* rmsValues = new float[n_chanels];
    for(int j = 0; j < n_chanels; j++){
        rmsValues[j] = chanels[j].getRMSValue();
    }
    float* result = new float[n_chanels];
    for(int j = 0; j < n_chanels; j++){
        result[j] = rmsValues[j];
    }
    delete[] rmsValues;
    */
    return true;
}
//==================================================================

class ES_tri_current_voltage_sensor: public EasySensor {
protected:
    // Variables de trabajo
    ES_chanel * chanels; // Canales de sensores
    int n_chanels = 6;

    float freq = -1.0;     // Frecuencia de la señal
    float currents[3] = {0.0,0.0,0.0};    // Corrientes equivalentes
    float voltages[3] = {0.0,0.0,0.0};    // Voltajes equivalentes
public:
    ES_tri_current_voltage_sensor(int * currentPins, int * voltagePins) : EasySensor(tri_current_voltage_sensor){
        // Asignar los pines a cada canal
        chanels = new ES_chanel[n_chanels];
        for(int i = 0; i < n_chanels/2; i++){
            chanels[i] = ES_chanel(currentPins[i], bufferSize);
            chanels[i+3] = ES_chanel(voltagePins[i], bufferSize);
        }
        //begin();
    };
    ~ES_tri_current_voltage_sensor(){};

    // Funciones principales y sobrescritura de virtuales
    bool begin();
    bool read();
    float * getSensorChanelRawData(int chanel){return chanels[chanel].readVoltages;};
    unsigned long * getSensorChanelRawTimes(int chanel){return chanels[chanel].readTimes;};
    float getSensorChanelOffset(int chanel){return chanels[chanel].getOffset();};

    // Funciones para acceder valores
    float * getCurrents(){return currents;};
    float * getVoltages(){return voltages;};
    float getFreq(){return freq;};

    // Funciones auxiliares
    bool setSensorFactors_CurrentChanels(float factorSensor, float valorMinLecturaRMS, float factorAmplificador);
    bool setSensorFactors_VoltageChanels(float factorSensor, float valorMinLecturaRMS, float factorAmplificador);
    void measureRMS();
    float * calcEquivalentCurrents(float *resultValues);
    float * calcEquivalentVoltages(float *voltageValues);
    float calcFreq(int NUM_MUESTRAS = bufferSize);
};


// Proceso de inicialización de los sensores
bool ES_tri_current_voltage_sensor::begin() {   // overrride
    // Inicializa cada canal
    for(int i = 0; i < n_chanels; i++){
        if(i<3) chanels[i].initChanel(factorSensorCorriente, factorAmplifidorCorriente, valorMinLecturaRMS_VI);  // Ejecuta el pinMode de cada canal y asigna factores (corriente)
        else    chanels[i].initChanel(factorSensorVoltaje, factorAmplifidorVoltaje, valorMinLecturaRMS_VI);   // Ejecuta el pinMode de cada canal y asigna factores (voltaje)
    }
    sensorStatus = true;     // variable que esta en la clase madre EasySensor
    return sensorStatus;
}

// Ejecuta tareas que hacer el muestreo de los sensores
bool ES_tri_current_voltage_sensor::read() {   // overrride
    measureRMS();
    calcEquivalentCurrents(this->currents);
    calcEquivalentVoltages(this->voltages);
    this->freq = calcFreq();

    // Agregar a sensorData los valores de currents, voltages y freq (este orden es importante al consultar los valores)
    float data[7] = {this->currents[0], this->currents[1], this->currents[2], this->voltages[0], this->voltages[1], this->voltages[2], this->freq};

    Serial.print("Frecuencia: ");
    Serial.println(freq);

    setSensorData(data);
    return true;
}

// Asigna a todos los canales de corrriente los factores de ajuste
bool ES_tri_current_voltage_sensor::setSensorFactors_CurrentChanels(float factorSensor, float valorMinLecturaRMS, float factorAmplificador){
    // ... por hacer
}

// Asigna a todos los canales de voltaje los factores de ajuste
bool ES_tri_current_voltage_sensor::setSensorFactors_VoltageChanels(float factorSensor, float valorMinLecturaRMS, float factorAmplificador){
    // ... por hacer
}

// Ejectua la medicion de los valores RMS de los sensores
void ES_tri_current_voltage_sensor::measureRMS(){
    multiMeasureRMS(chanels, n_chanels);    // esta medicion se hizo aqui para evitar hacerla en cada canal por aparte y ahorrar tiempo
}

// Calcula las corrientes equivalentes a partir del voltaje del RMS del ADC
float * ES_tri_current_voltage_sensor::calcEquivalentCurrents(float * resultValues){
    //float resultValues[3];
    for(int i = 0; i < 3; i++){
        resultValues[i] = chanels[i].getRMSValue() * (chanels[i].getFactorSensor()/chanels[i].getFactorAmplificador());
    }
    return resultValues;
}

// Calcula los voltajes equivalentes a partir del voltaje del RMS del ADC
float * ES_tri_current_voltage_sensor::calcEquivalentVoltages(float * voltageValues){
    //float voltageValues[3];
    for(int i = 0; i < 3; i++){
        voltageValues[i] = chanels[i+3].getRMSValue() * (chanels[i+3].getFactorSensor()/chanels[i+3].getFactorAmplificador());
    }
    return voltageValues;
}

// Calcula la frecuencia de la señal (Pendiente: actualizar metodos)
float ES_tri_current_voltage_sensor::calcFreq(int NUM_MUESTRAS){
    // Utilizar registros de tiempo para calcular la frecuencia de voltages
    int ch = 0;  // Canal leido para calcular la frecuencia
    unsigned long periodSum = 0;
    int zeroCrossings = 0;
    float freq_offset = chanels[ch].getOffset();
    bool lastPositive = chanels[ch].getReadVoltage(0) > freq_offset; // Initial state. Aqui this->chanels[0].offset es el centro de la señal
    
    /*
    
    unsigned long firstCrossingTime = 0;
    bool continueFreqMeasure = true;
    int i_m = 1;
    while(continueFreqMeasure){
        bool currentPositive = chanels[ch].getReadVoltage(i_m) > freq_offset;
        if (currentPositive != lastPositive) {
            if(zeroCrossings == 0) firstCrossingTime = periodSum;  // Registrar intervalo aleatorio inicial
            zeroCrossings++;
            periodSum += chanels[ch].getReadTime(i_m) - chanels[ch].getReadTime(i_m - 1);
            lastPositive = currentPositive;
        }
        if(zeroCrossings == 5) continueFreqMeasure = false;  // finalizar luego de cinco cruces
        i_m++;
    }
    periodSum -= firstCrossingTime;  // Eliminar periodo desconocido del primer cruce
    */

    
    for (int i = 1; i < NUM_MUESTRAS; i++) {
        bool currentPositive = chanels[ch].getReadVoltage(i) > freq_offset;
        /*
        Serial.print(0); Serial.print(" "); 
        Serial.print(freq_offset*100); Serial.print(" "); // Tabulador
        Serial.print(chanels[ch].getReadVoltage(i)*100); Serial.print(" "); 
        Serial.println(300);
        */        

        if (currentPositive != lastPositive) {
            zeroCrossings++;
            periodSum += chanels[ch].getReadTime(i) - chanels[ch].getReadTime(i - 1);
            lastPositive = currentPositive;
        }
    }
    
    if (zeroCrossings > 1) {
        //float averagePeriod = periodSum / (zeroCrossings - 1);
        //return 200000.0 / averagePeriod; // Convert period to frequency in Hz
        float freq = zeroCrossings*1000000.0 / (2*chanels[ch].getLastReadTime());  // freq = (cruces*10e6) / (2*tiempoPorLectura*bufferSize)
        if(chanels[ch].getRMSValue() == 0.0) return 0.0;  // Filtrar valores aleatorios
        else return freq;
    } else {
        return 0.0; // Not enough zero crossings to determine frequency
    }
    return -1.0;  // Valor de error
}


#endif // ES_tri_current_H