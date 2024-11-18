#ifndef WIDGET_H
#define WIDGET_H


class Widget{
protected:
    Adafruit_SSD1306 * display; // nombre del widget
    botones * botonesUI_w;          // Botones asociados al widget

    bool active = false; // Indica si el widget está activo
    bool drawingRequired = false; // Indica si se requiere dibujar el widget
public:
    Widget(Adafruit_SSD1306 * display, botones * botonesUI_w) : display(display), botonesUI_w(botonesUI_w) {};
    //virtual ~Widget(){}; // genera error en la compilación

    void activate(){active = true;}
    void desactivate(){active = false;}
    bool getStatus(){return active;}

    virtual char controls(){
        // Proceso de control mediante los botoness
        //------------------------------------------
        char botonPresionado = botonesUI_w->botonPresionado();  

        // Verificar si el usuario quiere salir del osciloscopio      
        if(botonPresionado == '4') {
            desactivate();
            display->clearDisplay();
            return '4';
        }
        //... (otros controles adicionales)

        return botonPresionado;
    };
    
    // Procceso generico de dibujo, puede ser sobreescrito
    virtual void draw(){
        display->display(); // Dibujar buffer creado en update()
    }

    virtual char update(){Serial.println("Error W1: Undefined update funtion in Widget.");};
};


#endif // WIDGET_H