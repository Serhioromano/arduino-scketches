#include <DMXSerial.h>
#include <Modbus.h>
#include <ModbusSerial.h>
#include <SoftwareSerial.h>
#include <TM1637Display.h>

TM1637Display display(2, 3);
SoftwareSerial mbSerial(10, 11);
ModbusSerial mb;

int aiParams[2] = {1001, 2001}; // Массив переменных
int aiStates[4];                // Кнопки 0 вверх, 1 вниз, 2 переключить параметре, 3 смена направления
int iParamNum;                  // Номер текущего параметра

const int btnUp = 8;
const int btnDown = 7;
const int btnSwitch = 5;
const int dsChangeDirection = 6; // смена направления LOW = дмх передает в модбас, HIGH = модбас передает в ДМХ 

void(* resetFunc) (void) = 0;

void setup()
{
    pinMode(btnUp, INPUT);
    pinMode(btnDown, INPUT);
    pinMode(btnSwitch, INPUT);
    pinMode(dsChangeDirection, INPUT);

    display.setBrightness(0x0a);
    mb.config(&mbSerial, 19200, 4);
    
    if(digitalRead(dsChangeDirection) == HIGH) {

    } else {
        mb.setSlaveId(1);
        for (int x = 1; x < 80; x++)
        {
            mb.addHreg(x);
        }
    }

}

void loop()
{
    /* Перегрузить устройство если смена направления конвертации */
    changeMode();

    /* Управление параметрами устройства */
    params();

    if(digitalRead(dsChangeDirection) == HIGH) {
        MB2Dmx();
    } else {
        Dmx2MB();
    }
}

void Dmx2MB() {
    DMXSerial.init(DMXReceiver);
    unsigned long lastPacket = DMXSerial.noDataSince();
    
    mb.task();
    if (lastPacket < 5000) {
        for (int i = aiParams[0] - 1000; i <= (aiParams[0] - 1000 + aiParams[1] - 2000); i++) {
            mb.Hreg(i - 1, DMXSerial.read(i));
        }
    } else {
        for (int i = aiParams[0] - 1000; i <= (aiParams[0] - 1000 + aiParams[1] - 2000); i++) {
            mb.Hreg(i - 1, 0);
        }
    }
}

void MB2Dmx() {

}

void changeMode(){
    if ((digitalRead(dsChangeDirection) == HIGH && aiStates[3] == LOW) || (digitalRead(dsChangeDirection) == LOW && aiStates[3] == HIGH))) {
        resetFunc();
    }
    aiStates[3] = digitalRead(dsChangeDirection);
}

void params() {
    /* Обрабатываем кнопку уменьшения значения */
    if (
        digitalRead(btnDown) == HIGH && 
        aiStates[1] == LOW &&
        (
            (iParamNum == 0 && aiParams[iParamNum] > 1001) || 
            (iParamNum == 1 && aiParams[iParamNum] > 2001)
        )
    ) {
        aiParams[iParamNum]--;
    }
    aiStates[1] = digitalRead(btnDown);

    /* Обрабатываем кнопку увеличения значения */
    if (
        digitalRead(btnUp) == HIGH && 
        aiStates[0] == LOW && 
        (
            (iParamNum == 0 && aiParams[iParamNum] < 1512) || 
            (iParamNum == 1 && aiParams[iParamNum] < 2512)
        )
    ) 
    {
        aiParams[iParamNum]++;
    }
    aiStates[0] = digitalRead(btnUp);

    /* Обрабатываем смены параметра */
    if (digitalRead(btnSwitch) == HIGH && aiStates[2] == LOW) {
        iParamNum = 1 - iParamNum;
    }
    aiStates[2] = digitalRead(btnSwitch);

    /* Выводим на экран */
    display.showNumberDec(aiParams[iParamNum]);
}
