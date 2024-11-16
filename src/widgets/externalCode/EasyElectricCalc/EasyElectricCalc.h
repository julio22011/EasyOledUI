// Ajustes de funciones
//--------------------------------------------------------------------------
const int NUM_MUESTRAS = 500;      // Número de muestras (ajustable)
const int TIEMPO_LECTURA = 100;    // Duración de la lectura en milisegundos
const int esperaUs = 100;          // Espera entre cada medicion en microsegundos

float factorSensor = 30.0;         // Factor del sensor (ajustable)
float valorMinLecturaRMS = 0.25;   // Valor mas bajo que se puede registrar. Para eliminar ruido.
float factorAmplificador = 10;     // Ajusta el valor según la amplificación del ccircuito de entrada



// Funcion para calcular la corriente RMS
// ----- Similar a la utilizada en: https://naylampmechatronics.com/blog/51_tutorial-sensor-de-corriente-ac-no-invasivo-sct-013.html
//--------------------------------------------------------------------------


// Funcion 1: Toma una serie de lecturas mediante un ADC y devuelve el voltaje RMS. Tambien asigna el offset a la direccion de memoria indicada.
//==================================================================
float medirValorRMS(int pinSensor, float * voltajes, float &offset){
    long tiempoInicial = millis();

    // Paso 1: Leer y guardar voltajes durante un tiempo específico
    int i = 0;
    while (millis() - tiempoInicial < TIEMPO_LECTURA && i < NUM_MUESTRAS) {
        voltajes[i] = analogRead(pinSensor) * (3.4 / 4096.0); // Conversión a voltaje
        i++;
        ets_delay_us(esperaUs);
    }

    // Paso 2: Calcular el offset (promedio de los voltajes leídos)
    offset = 0;
    for (int j = 0; j < i; j++) {
        offset += voltajes[j];
    }
    offset /= i; // Offset promedio

    // Paso 3: Calcular el RMS sin el offset
    float sumatoriaCuadrados = 0;
    for (int j = 0; j < i; j++) {
        float voltajeCorregido = voltajes[j] - offset;
        float corriente = voltajeCorregido * factorSensor; // Convertir voltaje a corriente
        sumatoriaCuadrados += sq(corriente);
    }
    float valorRMS = sqrt(sumatoriaCuadrados / i); // Calcular RMS

    if(valorRMS<valorMinLecturaRMS) return 0; // Filtro de valores muy bajos
    return valorRMS;
}

// Funcion 2: Utiliza la funcion 1 para adaptar el valor RMS a un valor de corriente equivalente segun el sensor
//==================================================================
float calcularCorrienteRMS(float &valorRMS, float &offset) {
    return valorRMS * (factorSensor/factorAmplificador); // convierte el valor de voltaje RMS medido a la corriente equivalente segun sensor 
}


// Funcion 3: Calcula la frecuencia segun los datos. 
//==================================================================
float calcularFrecuencia(float * voltajes, float offset, float period){
  // ... por hacer
}



