

#ifndef ES_channel_H
#define ES_channel_H


// La clase ES_channel permite crear varios canales de sensores con sus variables propias
class ES_chanel{
protected:
    int sensorPin;         // Pin del sensor
    float rmsValue = 0.0;  // Valor RMS del sensor
    int bufferSizeChanel;  // Tamaño del buffer de lecturas

    // Opciones
    float factorSensor;         // Factor del sensor (ajustable)
    float factorAmplificador;     // Ajusta el valor según la amplificación del ccircuito de entrada
    float valorMinLecturaRMS;   // Valor mas bajo que se puede registrar. Para eliminar ruido.
public:
    ES_chanel() = default; // Default constructor
    ES_chanel(int pin, int bufferSize_u): sensorPin(pin), bufferSizeChanel(bufferSize_u) {
        this->readVoltages = new float[this->bufferSizeChanel];         // Array para almacenar los valores del ADC (buffer)
        this->readTimes = new unsigned long[this->bufferSizeChanel];    // Tiempos de lectura de cada voltaje en us (buffer)
    };
    ~ES_chanel(){};

    // Variables de trabajo (publicas: pueden ser modificadas desde afuera)
    float offset;                 // Offset del sensor
    float * readVoltages;
    unsigned long * readTimes;

    // Funcion de inicialización (se asignan los valores iniciales)
    void initChanel(float factorSensor, float factorAmplificador, float valorMinLecturaRMS = 0.02){
        pinMode(sensorPin, INPUT);
        this->factorSensor = factorSensor;
        this->factorAmplificador = factorAmplificador;
        this->valorMinLecturaRMS = valorMinLecturaRMS;
    };

    // Funciones principales para retornar y modificar valores
    void setReadVoltage(float voltage, int n){
        this->readVoltages[n] = voltage;
    };
    void setReadTime(unsigned long time, int n){
        this->readTimes[n] = time;
    };
    void setOffset(float offset){
        this->offset = offset;
    };

    bool setSensorFactors(float factorSensor, float valorMinLecturaRMS, float factorAmplificador){
        this->factorSensor = factorSensor;
        this->valorMinLecturaRMS = valorMinLecturaRMS;
        this->factorAmplificador = factorAmplificador;
    };
    float * getSensorFactors(){
        float factors[3] = {factorSensor, valorMinLecturaRMS, factorAmplificador};
        return factors;
    };
    int getSensorPin(){return sensorPin;};
    float getFactorSensor(){return factorSensor;};
    float getValorMinLecturaRMS(){return valorMinLecturaRMS;};
    float getFactorAmplificador(){return factorAmplificador;};
    float getOffset(){return offset;};
    float getReadVoltage(int n){return readVoltages[n];};
    unsigned long getReadTime(int n){return readTimes[n];};

    bool setRMSValue(float value){this->rmsValue = value;};
    float getRMSValue(){return rmsValue;};
};


#endif // ES_channel_H