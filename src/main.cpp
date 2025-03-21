#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define ADDRESS 0x41 // slave address
#define PWM_ADDRESS 0x40 // PWM Module address

#define PWM_RES 16
#define PWM_CHANNEL 0   // ESP32 has 16 channels which can generate 16 independent waveforms
#define PWM_PERIOD 0.0025
#define PWM_FREQ 1/PWM_PERIOD   // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz
#define PWM_MAXDUTY (1 << PWM_RES) - 1

#define PWM_MIN 1000
#define PWM_MAX 2000

#define MAX_MOTORS 16

int dataSize = 5;
int pwmSize = 2;

typedef struct servo{
    int pin;
    int channel;
};

int motorCount = 0;
int servo[MAX_MOTORS];

const int DELAY_MS = 4;  // delay between fade increments
int _ledFadeStep = 5;

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(PWM_ADDRESS);

void setup(){
    Wire.begin(21, 22, 400000);
    Serial.begin(115200);
    Wire.setTimeOut(30);
    //ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    pca.begin();
    pca.setPWMFreq(PWM_FREQ);
    
    //ledcAttachPin(uint8_t pin, uint8_t channel);
    //ledcAttachPin(LED_OUTPUT_PIN, PWM_CHANNEL);
}

void loop(){  
    //ledcWrite(PWM_CHANNEL,MAX_DUTY_CYCLE);
    //ledcWrite(PWM_CHANNEL,0);
    // READING
    Wire.requestFrom(ADDRESS,dataSize);
    if(Wire.available() == dataSize) {
        int data[dataSize] = {0,0,0,0,0};
        for(int i = 0; i < dataSize; i++){
            data[i] = Wire.read();
        }
        for(int i = 0; i < dataSize; i++){
            Serial.printf("%x" ,data[i]);
            Serial.print("  ");
        }
        int32_t combinedData = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3]);
        Serial.print("Speed: ");
        Serial.println(combinedData);
        //Serial.printf("Data received: %d\n", data);
        //Serial.print("Data received: ");
        //Serial.println(data);
    }
    else Serial.println("error");
    Wire.flush();
    int fwd = random(-1,1);
    int dutyCyclePercentage = map(fwd, -1, 1, 0, 100); // fwd will be 
    int pwmValue = map(dutyCyclePercentage, 0, 100, 0, 4095);

    pca.setPWM(motorCount, PWM_MIN, pwmValue);
     
}
