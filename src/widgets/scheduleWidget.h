

#ifndef Sch_WIDGET_H
#define Sch_WIDGET_H


class ScheduleWidget: public Widget{
    
protected:
    int hour1 = 0;
    int minute1 = 0;
    int hour2 = 0;
    int minute2 = 0;
    int am_pm1 = 0; // 0 = AM, 1 = PM
    int am_pm2 = 0; // 0 = AM, 1 = PM

private:
    void drawShedule();
    void saveSchedule();
    void loadSchedule();
    void deleteSchedule();
public:
    ScheduleWidget(Adafruit_SSD1306 * display, botones * botonesUI_w) : Widget(display, botonesUI_w) {};
    char update();
};


void ScheduleWidget::drawShedule(){
    // Dibujar la pantalla de programación
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    display->println("Programacion");
    
    // Mostrar hora y minuto
    display->setCursor(0, 10);
    display->print("Hora 1: ");
    display->print(hour1);
    display->print(":");
    display->print(minute1);
    
    // Mostrar AM/PM
    if(am_pm1 == 0) {
        display->print(" AM");
    } else {
        display->print(" PM");
    }
    
    // Repetir para la segunda hora
    display->setCursor(0, 20);
    display->print("Hora 2: ");
    display->print(hour2);
    display->print(":");
    display->print(minute2);
    
    if(am_pm2 == 0) {
        display->print(" AM");
    } else {
        display->print(" PM");
    }
    
    // Actualizar pantalla
    display->display();
}

void ScheduleWidget::saveSchedule(){
    //
}

void ScheduleWidget::loadSchedule(){
    //
}

void ScheduleWidget::deleteSchedule(){
    //
}


char ScheduleWidget::update(){
        // Proceso de actualización de datos
    if(active){
        display->clearDisplay();                   // Borrar la pantalla antes de redibujar el buffer
        drawShedule();                             // Proceso para dibujar la programación

        draw();                                    // aplicar acciones de dibujo si son requeridas (envia el buffer al display)
        char controles = controls();               // aplicar acciones de control si son requeridas y retorna char de resultado
        if(controles == '1'){
            //
        }
        else if (controles == '2'){
            //
        }

        return controls();                         // aplicar acciones de control si son requeridas y retorna char de resultado
    }
}



#endif