

#ifndef Sch_WIDGET_H
#define Sch_WIDGET_H

#include <Preferences.h>
#include <vector>

enum ScheduleAction {
    INCREASE_HOUR_ON,
    INCREASE_HOUR_OFF,
    DECREASE_HOUR_ON,
    DECREASE_HOUR_OFF,
    INCREASE_MINUTE_ON,
    INCREASE_MINUTE_OFF,
    DECREASE_MINUTE_ON,
    DECREASE_MINUTE_OFF,
    TOGGLE_AM_PM_ON,
    TOGGLE_AM_PM_OFF,
    ADD_SCHEDULE,
    LOAD_SCHEDULE,
    SAVE_SCHEDULE,
    REMOVE_SCHEDULE,
    CHANGE_SELECTED
};

enum ScheduleOpction {
    HOUR_ON,
    MINUTE_ON,
    AM_PM_ON,
    HOUR_OFF,
    MINUTE_OFF,
    AM_PM_OFF,
    SAVE_BTN,
    REMOVE_BTN
};

class ScheduleWidget: public Widget{
    // Widget de programación
    // Permite programar dos horarios para encender y apagar un dispositivo
    // Se puede guardar en la memoria permanente
    // Se puede cargar desde la memoria permanente
    // Se puede eliminar desde la memoria permanente

protected:
    int hour1 = 0;
    int minute1 = 0;
    int hour2 = 0;
    int minute2 = 0;
    int am_pm1 = 0; // 0 = AM, 1 = PM
    int am_pm2 = 0; // 0 = AM, 1 = PM

    int currentSch = 0; // Indica el horario actual a editar
    int selected = HOUR_ON; // Indica el parametro seleccionado

    bool pendingDrawingChanges = true; // Indica si se requiere dibujar el widget

    // Estructura para almacenar los horarios
    struct Schedule {
        int hour;
        int minute;
        int am_pm; // 0 = AM, 1 = PM
    };
    std::vector<Schedule> schedules; // Vector para almacenar los horarios



private:
    void drawSchedule();
    void saveSchedule();
    void loadInClassMemory(int scheduleDuplexIndex);
    void loadSchedule();
    void deleteSchedule();
    void addSchedule();
    void changeCurrentSchedule(int schNumber);
    void changeSelected(int newSelected);
    void notification(const String& message);
public:
    ScheduleWidget(Adafruit_SSD1306 * display, botones * botonesUI_w) : Widget(display, botonesUI_w) {
        // Load the saved shcedule from permanent memory
        this->loadSchedule();
    };
    void Action(ScheduleAction act);
    void addEmptySchedule();
    void removeSchedule(int schNumber);
    int * getSchedule(int schNumber);
    char update();
};


void ScheduleWidget::drawSchedule(){

    // Dibujar la pantalla de programación
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    //display->println("Horario:");
    display->print("Horario:");
    display->println(currentSch);
    display->drawLine(1, 10, display->width()-15, 10, SSD1306_WHITE);
    
    // Mostrar hora y minuto
    display->setCursor(0, 20);
    display->print("ON:  ");
    //printUnderlined("ON", 0, 10, 1, display);
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
    display->setCursor(0, 40);
    display->print("OFF: ");
    display->print(hour2);
    display->print(":");
    display->print(minute2);
    
    if(am_pm2 == 0) {
        display->print(" AM");
    } else {
        display->print(" PM");
    }

    // Dibujar linea sobre el parametro seleccionado
    switch (selected) {
    case HOUR_ON:
        display->drawLine(27, 28, 37, 28, SSD1306_WHITE);
        break;
    case MINUTE_ON:
        display->drawLine(42, 28, 52, 28, SSD1306_WHITE);
        break;
    case AM_PM_ON:
        display->drawLine(55, 28, 65, 28, SSD1306_WHITE);
        break;

    case HOUR_OFF:
        display->drawLine(27, 48, 37, 48, SSD1306_WHITE);
        break;
    case MINUTE_OFF:
        display->drawLine(42, 48, 52, 48, SSD1306_WHITE);
        break;
    case AM_PM_OFF:
        display->drawLine(55, 48, 65, 48, SSD1306_WHITE);
        break;
    case SAVE_BTN:
        display->drawLine(0, 48, 127, 48, SSD1306_WHITE);
        display->setCursor(80, 50);
        display->print("Guardar");
        break;
    case REMOVE_BTN:
        display->drawLine(0, 48, 127, 48, SSD1306_WHITE);
        display->setCursor(80, 50);
        display->print("Borrar");
        break;
    default:
        break;
    }
    
    // Actualizar pantalla
    //display->display();
}

void ScheduleWidget::addEmptySchedule(){
    // Agregar un nuevo horario vacío al vector
    Schedule scheduleOn;  // Horario de encendido
    Schedule scheduleOff; // Horario de apagado

    // Asignar valores a los horarios
    scheduleOn.hour = 0;
    scheduleOn.minute = 0;
    scheduleOn.am_pm = 0;

    scheduleOff.hour = 0;
    scheduleOff.minute = 0;
    scheduleOff.am_pm = 0;

    // Agregar ambos horarios al vector
    schedules.push_back(scheduleOn);
    schedules.push_back(scheduleOff);

    // Guardar el horario en la memoria permanente
    saveSchedule();
    notification("Horario creado");
}

void ScheduleWidget::removeSchedule(int schNumber){
    // Eliminar un horario del vector
    if(schNumber < 0 || schNumber >= schedules.size() / 2) {
        Serial.println("Error: Invalid schedule index or no schedules added yet.");
    } else{
        schedules.erase(schedules.begin() + schNumber * 2, schedules.begin() + schNumber * 2 + 2);
        saveSchedule();
        notification("Horario eliminado");
    }
}

int * ScheduleWidget::getSchedule(int schNumber){
    // Obtener un horario del vector
    if(schNumber < 0 || schNumber >= schedules.size() / 2) {
        Serial.println("Error: Invalid schedule index or no schedules added yet.");
        return NULL;
    } else{
        int * schedule = new int[6];
        schedule[0] = schedules[schNumber * 2].hour;
        schedule[1] = schedules[schNumber * 2].minute;
        schedule[2] = schedules[schNumber * 2].am_pm;
        schedule[3] = schedules[schNumber * 2 + 1].hour;
        schedule[4] = schedules[schNumber * 2 + 1].minute;
        schedule[5] = schedules[schNumber * 2 + 1].am_pm;
        return schedule;
    }
}


void ScheduleWidget::addSchedule(){
    // Agregar un nuevo horario al vector
    Schedule scheduleOn;  // Horario de encendido
    Schedule scheduleOff; // Horario de apagado

    // Asignar valores a los horarios
    scheduleOn.hour = hour1;
    scheduleOn.minute = minute1;
    scheduleOn.am_pm = am_pm1;

    scheduleOff.hour = hour2;
    scheduleOff.minute = minute2;
    scheduleOff.am_pm = am_pm2;

    // Agregar ambos horarios al vector
    schedules.push_back(scheduleOn);
    schedules.push_back(scheduleOff);

    // Guardar el horario en la memoria permanente
    saveSchedule();
    notification("Horario agregado");
}

void ScheduleWidget::changeCurrentSchedule(int schNumber){
    // Cambiar el horario actual a editar
    if(schNumber < 0 || schNumber >= schedules.size() / 2) {
        Serial.println("Error: Invalid schedule index or no schedules added yet.");
    } else{
        schedules[schNumber * 2].hour = hour1;
        schedules[schNumber * 2].minute = minute1;
        schedules[schNumber * 2].am_pm = am_pm1;
        schedules[schNumber * 2 + 1].hour = hour2;
        schedules[schNumber * 2 + 1].minute = minute2;
        schedules[schNumber * 2 + 1].am_pm = am_pm2;
    }
    pendingDrawingChanges = true; // Indicar que se requiere redibujar
}

void ScheduleWidget::changeSelected(int newSelected = -1){
    if(newSelected == -1){
        // Cambiar el parametro seleccionado
        selected++;
        if(selected > REMOVE_BTN) selected = HOUR_ON; // Volver al primer parametro
    } else {
        selected = newSelected;
    }
    pendingDrawingChanges = true; // Dejar activado para volver a dibujar al reingresar
}

void ScheduleWidget::saveSchedule(){
    // Guardar en la memoria permanente el horario digitado
    Preferences preferences;
    if (preferences.begin("schedule", false)) {
        int scheduleCount = schedules.size() / 2; // Cada par representa un encendido y apagado
        preferences.putInt("scheduleCount", scheduleCount);
        for (int i = 0; i < scheduleCount; ++i) {
            // Guardar horario de encendido
            preferences.putInt(("on_hour" + String(i)).c_str(), schedules[i * 2].hour);
            preferences.putInt(("on_minute" + String(i)).c_str(), schedules[i * 2].minute);
            preferences.putInt(("on_am_pm" + String(i)).c_str(), schedules[i * 2].am_pm);

            // Guardar horario de apagado
            preferences.putInt(("off_hour" + String(i)).c_str(), schedules[i * 2 + 1].hour);
            preferences.putInt(("off_minute" + String(i)).c_str(), schedules[i * 2 + 1].minute);
            preferences.putInt(("off_am_pm" + String(i)).c_str(), schedules[i * 2 + 1].am_pm);
        }
        preferences.end();
        Serial.println("Schedules saved:");
        for (int i = 0; i < scheduleCount; ++i) {
            Serial.print("Schedule ");
            Serial.print(i + 1);
            Serial.print(" ON: ");
            Serial.print(schedules[i * 2].hour);
            Serial.print(":");
            Serial.print(schedules[i * 2].minute);
            Serial.print(" AM/PM: ");
            Serial.println(schedules[i * 2].am_pm);

            Serial.print("Schedule ");
            Serial.print(i + 1);
            Serial.print(" OFF: ");
            Serial.print(schedules[i * 2 + 1].hour);
            Serial.print(":");
            Serial.print(schedules[i * 2 + 1].minute);
            Serial.print(" AM/PM: ");
            Serial.println(schedules[i * 2 + 1].am_pm);
        }
    } else {
        Serial.println("Failed to initialize Preferences.");
    }
    preferences.end();
}

void ScheduleWidget::notification(const String& message){
    // Mostrar mensaje
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    display->println(message);
    display->display();
    delay(2000); // Esperar 2 segundos para mostrar el mensaje
    display->clearDisplay();
}

void ScheduleWidget::loadInClassMemory(int scheduleDuplexIndex = 0){
    // Poner en memoria de la calse el horario seleccionado
    if(scheduleDuplexIndex < 0 || scheduleDuplexIndex >= schedules.size() / 2) {
        Serial.println("Error: Invalid schedule index or no schedules added yet.");
    } else{
        hour1 = schedules[scheduleDuplexIndex * 2].hour;
        minute1 = schedules[scheduleDuplexIndex * 2].minute;
        am_pm1 = schedules[scheduleDuplexIndex * 2].am_pm;
        hour2 = schedules[scheduleDuplexIndex * 2 + 1].hour;
        minute2 = schedules[scheduleDuplexIndex * 2 + 1].minute;
        am_pm2 = schedules[scheduleDuplexIndex * 2 + 1].am_pm;
        currentSch = scheduleDuplexIndex; // Cambiar el horario actual a editar
    }

}

void ScheduleWidget::loadSchedule(){
    // Cargar los datos de la memoria permanente
    Preferences preferences;
    preferences.begin("schedule", true);
    int scheduleCount = preferences.getInt("scheduleCount", 0);
    schedules.clear(); // Limpiar el vector de horarios

    // Retornar si no hay horarios guardados y cargar valores por defecto
    if (scheduleCount == 0) {
        Serial.println("No schedules found.");
        preferences.end();

        this->Action(ADD_SCHEDULE);  // Cambiar el canal de medición
        Serial.println("Default schedule loaded to permanent memory.");
        return;
    }
    for (int i = 0; i < scheduleCount; ++i) {
        Schedule scheduleOn;  // Horario de encendido
        Schedule scheduleOff; // Horario de apagado

        // Cargar horario de encendido
        scheduleOn.hour = preferences.getInt(("on_hour" + String(i)).c_str(), 0);
        scheduleOn.minute = preferences.getInt(("on_minute" + String(i)).c_str(), 0);
        scheduleOn.am_pm = preferences.getInt(("on_am_pm" + String(i)).c_str(), 0);

        // Cargar horario de apagado
        scheduleOff.hour = preferences.getInt(("off_hour" + String(i)).c_str(), 0);
        scheduleOff.minute = preferences.getInt(("off_minute" + String(i)).c_str(), 0);
        scheduleOff.am_pm = preferences.getInt(("off_am_pm" + String(i)).c_str(), 0);

        // Agregar ambos horarios al vector
        schedules.push_back(scheduleOn);
        schedules.push_back(scheduleOff);
    }
    preferences.end();

    // Show the loaded schedules
    Serial.println("Schedules loaded:");
    for (int i = 0; i < schedules.size(); i += 2) {
        Serial.print("Schedule ");
        Serial.print((i / 2) + 1);
        Serial.print(" ON: ");
        Serial.print(schedules[i].hour);
        Serial.print(":");
        Serial.print(schedules[i].minute);
        Serial.print(" AM/PM: ");
        Serial.println(schedules[i].am_pm);

        Serial.print("Schedule ");
        Serial.print((i / 2) + 1);
        Serial.print(" OFF: ");
        Serial.print(schedules[i + 1].hour);
        Serial.print(":");
        Serial.print(schedules[i + 1].minute);
        Serial.print(" AM/PM: ");
        Serial.println(schedules[i + 1].am_pm);
    }

    // Cargar el primer horario en la memoria de la clase
    loadInClassMemory(0);
}

// For deleting all the schedules from the permanent memory
void ScheduleWidget::deleteSchedule(){
    // Eliminar los horarios de la memoria permanente
    Preferences preferences;
    preferences.begin("schedule", false);
    int scheduleCount = preferences.getInt("scheduleCount", 0);
    for (int i = 0; i < scheduleCount; ++i) {
        preferences.remove(("on_hour" + String(i)).c_str());
        preferences.remove(("on_minute" + String(i)).c_str());
        preferences.remove(("on_am_pm" + String(i)).c_str());

        preferences.remove(("off_hour" + String(i)).c_str());
        preferences.remove(("off_minute" + String(i)).c_str());
        preferences.remove(("off_am_pm" + String(i)).c_str());
    }
    preferences.remove("scheduleCount");
    preferences.end();
    Serial.println("Schedules deleted");
}

void ScheduleWidget::Action(ScheduleAction act){
    // Incrementar el horario de encendido y apagado
    if(act == INCREASE_HOUR_ON){
        if (this->hour1 == 23) {
            this->hour1 = 0;
        } else {
            this->hour1++;
        }
    } else if(act == INCREASE_HOUR_OFF){
        if (this->hour2 == 23) {
            this->hour2 = 0;
        } else {
            this->hour2++;
        }
    } else if(act == DECREASE_HOUR_ON){
        if (this->hour1 == 0) {
            this->hour1 = 23;
        } else {
            this->hour1--;
        }
    } else if(act == DECREASE_HOUR_OFF){
        if (this->hour2 == 0) {
            this->hour2 = 23;
        } else {
            this->hour2--;
        }
    } else if(act == INCREASE_MINUTE_ON){
        if (this->minute1 == 59) {
            this->minute1 = 0;
        } else {
            this->minute1++;
        }
    } else if(act == INCREASE_MINUTE_OFF){
        if (this->minute2 == 59) {
            this->minute2 = 0;
        } else {
            this->minute2++;
        }
    } else if(act == DECREASE_MINUTE_ON){
        if (this->minute1 == 0) {
            this->minute1 = 59;
        } else {
            this->minute1--;
        }
    } else if(act == DECREASE_MINUTE_OFF){
        if (this->minute2 == 0) {
            this->minute2 = 59;
        } else {
            this->minute2--;
        }
    } else if(act == TOGGLE_AM_PM_ON){
        this->am_pm1 = !this->am_pm1; // Cambiar AM/PM
    } else if(act == TOGGLE_AM_PM_OFF){
        this->am_pm2 = !this->am_pm2; // Cambiar AM/PM
    }
    else if(act == ADD_SCHEDULE){
        this->addSchedule(); // Guardar el horario en la memoria permanente
    }
    else if(act == LOAD_SCHEDULE){
        this->loadSchedule(); // Cargar el horario de la memoria permanente
    }
    else if(act == SAVE_SCHEDULE){
        this->saveSchedule(); // Guardar el horario en la memoria permanente
        notification("Horario guardado");
    }
    else if(act == REMOVE_SCHEDULE){
        this->removeSchedule(currentSch); // Eliminar el horario de la memoria permanente
        this->desactivate(); // Desactivar el widget
    }
    else if (act == CHANGE_SELECTED){
        this->changeSelected(); // Cambiar el parametro seleccionado
    }
    else{
        Serial.println("Error: Invalid action");
    }
}


char ScheduleWidget::update(){
        // Proceso de actualización de datos
    if(active){
        if(pendingDrawingChanges){
            display->clearDisplay();                   // Borrar la pantalla antes de redibujar el buffer
            drawSchedule();                             // Proceso para dibujar la programación
            draw();
            pendingDrawingChanges = false; // Resetear el flag de dibujo
        }

        char controles = controls();               // aplicar acciones de control si son requeridas y retorna char de resultado
        if(controles == '1'){
            switch (selected) {
            case HOUR_ON:
                this->Action(INCREASE_HOUR_ON);  // Cambiar el canal de medición
                break;
            case MINUTE_ON:
                this->Action(INCREASE_MINUTE_ON);  // Cambiar el canal de medición
                break;
            case AM_PM_ON:

                this->Action(TOGGLE_AM_PM_ON);  // Cambiar el canal de medición
                break;
            case HOUR_OFF:

                this->Action(INCREASE_HOUR_OFF);  // Cambiar el canal de medición
                break;
            case MINUTE_OFF:
                this->Action(INCREASE_MINUTE_OFF);  // Cambiar el canal de medición
                break;
            case AM_PM_OFF:
                this->Action(TOGGLE_AM_PM_OFF);  // Cambiar el canal de medición
                break;
            case SAVE_BTN:
                this->Action(SAVE_SCHEDULE);  // Cambiar el canal de medición
                pendingDrawingChanges = true; // Dejar activado para volver a dibujar al reingresar
                break;
            default:
                break;
            }
            changeCurrentSchedule(0); // Cambiar el horario actual a editar
        }
        else if (controles == '2'){
            switch(selected){
                case HOUR_ON:
                    this->Action(DECREASE_HOUR_ON);  // Cambiar el canal de medición
                    break;
                case MINUTE_ON:
                    this->Action(DECREASE_MINUTE_ON);  // Cambiar el canal de medición
                    break;
                case AM_PM_ON:
                    this->Action(TOGGLE_AM_PM_ON);  // Cambiar el canal de medición
                    break;
                case HOUR_OFF:
                    this->Action(DECREASE_HOUR_OFF);  // Cambiar el canal de medición
                    break;
                case MINUTE_OFF:
                    this->Action(DECREASE_MINUTE_OFF);  // Cambiar el canal de medición
                    break;
                case AM_PM_OFF:
                    this->Action(TOGGLE_AM_PM_OFF);  // Cambiar el canal de medición
                    break;
                case SAVE_BTN:
                    this->Action(SAVE_SCHEDULE);  // Cambiar el canal de medición
                    pendingDrawingChanges = true; // Dejar activado para volver a dibujar al reingresar
                    break;
                case REMOVE_BTN:
                    this->Action(REMOVE_SCHEDULE);  // Cambiar el canal de medición
                    pendingDrawingChanges = true; // Dejar activado para volver a dibujar al reingresar
                    break;
                default:
                    break;
            }
            changeCurrentSchedule(0); // Cambiar el horario actual a editar
        }
        else if (controles == '3'){
            //this->Action(DELETE_SCHEDULE);  // Cambiar el canal de medición
            this->Action(CHANGE_SELECTED);  // Cambiar el canal de medición
        }
        else if (controles == '4'){
            //this->Action(SAVE_SCHEDULE);  // Cambiar el canal de medición
            //pendingDrawingChanges = true; // Dejar activado para volver a dibujar al reingresar
        }

        return controls();                         // aplicar acciones de control si son requeridas y retorna char de resultado
    }
}


#endif