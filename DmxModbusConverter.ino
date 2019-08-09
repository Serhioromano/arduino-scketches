#include <DMXSerial.h>
#include <Modbus.h>
#include <ModbusSerial.h>
#include <SoftwareSerial.h>
#include <TM1637Display.h>

TM1637Display display(2, 3);
SoftwareSerial mbSerial(10, 11);
ModbusSerial mb;

int aiParams[2] = {1001, 2001}; // Массив переменных
int aiBtn[3];                   // Кнопки
int iParamNum;                  // Номер текущего параметра

const int btnUp = 8;
const int btnDown = 7;
const int btnSwitch = 5;

void setup()
{
    display.setBrightness(0x0a);
    mb.config(&mbSerial, 19200, 12);
    mb.setSlaveId(1);
    for (int x = 1; x < 80; x++)
    {
        mb.addHreg(x);
    }

    pinMode(btnUp, INPUT);
    pinMode(btnDown, INPUT);
    pinMode(btnSwitch, INPUT);
}

void loop()
{
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

    /* Обрабатываем кнопку увеличения значения */
    if (digitalRead(btnUp) == HIGH && aiBtn[0] == LOW) {
        if ((iParamNum == 0 && aiParams[iParamNum] < 1512) || (iParamNum == 1 && aiParams[iParamNum] < 2512)) {
            aiParams[iParamNum]++;
        }
    }
    aiBtn[0] = digitalRead(btnUp);

    /* Обрабатываем кнопку уменьшения значения */
    if (digitalRead(btnDown) == HIGH && aiBtn[1] == LOW) {
        if ((iParamNum == 0 && aiParams[iParamNum] > 1001) || (iParamNum == 1 && aiParams[iParamNum] > 2001)) {
            aiParams[iParamNum]--;
        }
    }
    aiBtn[1] = digitalRead(btnDown);

    /* Обрабатываем смены параметра */
    if (digitalRead(btnSwitch) == HIGH && aiBtn[2] == LOW) {
        iParamNum = 1 - iParamNum;
    }
    aiBtn[2] = digitalRead(btnSwitch);

    display.showNumberDec(aiParams[iParamNum]);
}
