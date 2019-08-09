
#include <DMXSerial.h>
#include <Modbus.h>
#include <ModbusSerial.h>
#include <SoftwareSerial.h>
#include <TM1637Display.h>
const int CLK = 2;//Set the CLK pin connection to the display
const int DIO = 3; //Set the DIO pin connection to the display
TM1637Display display(CLK, DIO);
int NumReg[2] = {1001, 2001}; //переменная регистра
ModbusSerial mb;
SoftwareSerial mbSerial(10,11);
int btnUp, btnDown, btnSwch = 0;
int btnUpM, btnDownM, btnSwchM = 0;
int Num_index;


const int RedPin =    9;  // PWM output pin for Red Light.
const int GreenPin =  6;  // PWM output pin for Green Light.
//const int BluePin =   5;  // PWM output pin for Blue Light.

#define RedDefaultLevel   100
#define GreenDefaultLevel 200
#define BlueDefaultLevel  255

const int button_up = 8;   // переключение регистра вверх
const int button_down = 7; // переключение регистра вниз
const int button_var = 5;
   
void setup () {
    display.setBrightness(0x0a);
    mb.config(&mbSerial, 19200, 12);
    mb.setSlaveId(1);
    for (int x = 1; x < 80; x++) {
        mb.addHreg(x);
    }  
    
  
    pinMode(RedPin,   OUTPUT); // sets the digital pin as output
    pinMode(GreenPin, OUTPUT);
    // pinMode(BluePin,  OUTPUT);
    pinMode(button_up, INPUT);
    pinMode(button_down, INPUT);
    pinMode(button_down, INPUT);
}


void loop() {
    DMXSerial.init(DMXReceiver);
    mb.task();
    unsigned long lastPacket = DMXSerial.noDataSince();
    btnUp = digitalRead(button_up);
    btnDown = digitalRead(button_down);
    btnSwch = digitalRead(button_var);

    if (lastPacket < 5000) {
        analogWrite(RedPin,   DMXSerial.read(1));
        analogWrite(GreenPin, DMXSerial.read(2));
        //analogWrite(BluePin,  DMXSerial.read(3));
    } else {
        analogWrite(RedPin,   RedDefaultLevel);
        analogWrite(GreenPin, GreenDefaultLevel);
        //analogWrite(BluePin,  BlueDefaultLevel);
    } 

    if (btnUp == HIGH && btnUpM == LOW) {
      if ((Num_index==0 && NumReg[Num_index]<1512 ) || (Num_index==1 && NumReg[Num_index] <2512)) {
         NumReg[Num_index]++;
      }
    }
    btnUpM = btnUp;

    if (btnDown == HIGH && btnDownM == LOW) {
        if ((Num_index==0 && NumReg[Num_index]>1001 ) || (Num_index==1 && NumReg[Num_index] >2001)) {
            NumReg[Num_index]--;
        }
    }
    btnDownM = btnDown;

    if (btnSwch == HIGH && btnSwchM == LOW){
        Num_index = 1 - Num_index;
    }
    btnSwchM = btnSwch;

    for (int i = NumReg[0]-1000; i <= (NumReg[0]-1000 + NumReg[1]-2000); i++) {
        mb.Hreg(i - 1, DMXSerial.read(i));
    }

    display.showNumberDec(NumReg[Num_index]);
}
