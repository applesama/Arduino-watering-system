#include <Arduino.h>
#line 1 "f:\\WaterArduino\\watering\\watering.ino"

#include <dht11.h>
#include <U8g2lib.h>
#include "ArduinoSTL.h"
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

using namespace std;
#define DHT11PIN 5 //define the pin2 is the airTempHumidity sensor
#define A 3        //has been changed with B, if any bugs appear with encoder, check this
#define B 2
#define C 18
dht11 DHT11; //create the dht11 sensor

class AvaliablePort
{
public:
    void setToAvaliable(int port)
    {
        mAvaliableSet[port] = true;
    }

    int getHumidityPort()
    {
        for (int i = 0; i < 4; i++)
        {
            if (mAvaliableSet[i] == true)
            {
                mAvaliableSet[i] = false; //This port not is being used
                return mHumidityPort[i];
            }
        }
        return -1;
    }

    int getServoPort()
    {
        for (int i = 0; i < 4; i++)
        {
            if (mAvaliableSet[i] == true)
            {
                return mServoPort[i - 1]; //Return last unavaliable port, because it has just been truned to false as getHumidityPort was called.
            }
            else if (mAvaliableSet[3] == false)
            {
                return mServoPort[3];
            }
        }
        return -1;
    }

    char *getSensorName()
    {
        for (int i = 0; i < 4; i++)
        {
            if (mAvaliableSet[i] == true)
            {
                return mSensors[i - 1];
            }
            else if (mAvaliableSet[3] == false)
            {
                return mSensors[3];
            }
        }
        return "-1";
    }

    void freeThePort(int HumPort)
    {
        for (int i = 0; i < 4; i++)
        {
            if (mHumidityPort[i] == HumPort)
            {
                mAvaliableSet[i] = true;
            }
        }
    }

    AvaliablePort(); //constructor
private:
    volatile bool mAvaliableSet[4] = {true, true, true, true}; //Each of the set repersent two ports that one plantsensor need, one analog, one digits
    const int mHumidityPort[4] = {14, 15, 16, 17};             //All ports for humidity sensors
    const int mServoPort[4] = {8, 9, 10, 11};                  //All ports for servo motors
    const char *mSensors[4] = {"Sensor1", "Sensor2", "Sensor3", "Sensor4"};
};

AvaliablePort::AvaliablePort()
{ //not sure if it work, (pinmode outside of the main function)
    for (int i = 0; i < 4; i++)
    {
        pinMode(mHumidityPort[i], INPUT);
        pinMode(mServoPort[i], OUTPUT);
    }
}

AvaliablePort arduinoPort; //instantiating it

//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // All Boards without Reset of the Display
class PlantSensor
{
public:
    void setTempertureLowerLimit(int lowerLimit)
    {
        mTemperatureInterval[0] = lowerLimit;
    }

    void setTempertureUpperLimit(int upperLimit)
    {
        mTemperatureInterval[1] = upperLimit;
    }

    void setHumidityLowerLimit(int lowerLimit)
    {
        mHumidityInterval[0] = lowerLimit;
    }

    void setHumidityUpperLimit(int upperLimit)
    {
        mHumidityInterval[1] = upperLimit;
    }

    void setCurrentHumidity()
    {
        mCurrentHumidity = analogRead(mSensorPort);
    }

    void ResetDaysSinceLastWatering()
    {
        mDaysSinceLastWatering = 0;
    }

    void DaysPlusOne()
    {
        mDaysSinceLastWatering++;
    }

    void setSensorPort(int port)
    {
        mSensorPort = port;
        pinMode(mSensorPort, INPUT);
    }

    void setServoPort(int port)
    {
        mServoPort = port;
        pinMode(mServoPort, OUTPUT);
    }

    void watering()
    { // loop will keep call this function all the time
        if (mWaterFlag)
        {
            digitalWrite(mServoPort, LOW);
            delay(5000);
            digitalWrite(mServoPort, HIGH);
            mWaterFlag = false;
        }
    }

    void setWater()
    { //set it to water when we want
        mWaterFlag = true;
    }

    void setRecord(bool flag)
    {
        mRecordFlag = flag;
    }

    bool getRecordFlag()
    {
        return mRecordFlag;
    }

    /*void setOption(bool flag){
        mOptionFlag = flag;
    }

    bool getOptionFlag(){
        return mOptionFlag;
    }*/

    int getTempertureLowerLimit()
    {
        return mTemperatureInterval[0];
    }

    int getTempertureUpperLimit()
    {
        return mTemperatureInterval[1];
    }

    int getHumidityLowerLimit()
    {
        return mHumidityInterval[0];
    }

    int getHumidityUpperLimit()
    {
        return mHumidityInterval[1];
    }

    int getCurrentHumidity()
    {
        return mCurrentHumidity;
    }
    int setCurrentTemperture(int temp)
    {
        return mCurrentTemperture;
    }

    int getDaysSinceLastWatering()
    {
        return mDaysSinceLastWatering;
    }

    int getSensorPort()
    {
        return mSensorPort;
    }

    int getServoPort()
    {
        return mServoPort;
    }

    char *getName()
    {
        return mName;
    }

    PlantSensor(int port2, char *name, int port1);
    ~PlantSensor();

private:
    int mTemperatureInterval[2], mHumidityInterval[2], mCurrentTemperture, mCurrentHumidity, mDaysSinceLastWatering;
    char *mName;
    int mSensorPort, mServoPort;
    volatile bool mWaterFlag = false;
    volatile bool mRecordFlag = false;
    volatile bool mOptionFlag = false;
};

PlantSensor::PlantSensor(int port2, char *name, int port1) //name means which plant it is
{                                                          //first is sensor port and second one is servo
    mSensorPort = port1;
    mServoPort = port2;
    mCurrentTemperture = 0;
    mCurrentHumidity = 0;
    mDaysSinceLastWatering = 0;
    mName = name;
}

PlantSensor::~PlantSensor()
{
    //arduinoPort.freeThePort(mSensorPort); //Make the port be avaliable
}

vector<PlantSensor> sensors;

bool enterMenu = false; //if enter the menu
const char *mainMenuItem[4] = {"Manual Watering", "Auto-Watering setting", "Record", "Back"};
const char *menuItemFor6[5] = {"UpperTemputerture", "LowerTemputerture", "UpperHumidity", "LowerHumidity", "Back"};

uint8_t currentMenu = 1; //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor
uint8_t currentPage = 1;
uint8_t currentItem = 1;
uint8_t currentItemForLastPage = 1; //record which item selected in last item;

uint8_t sensorThatCurrentHave = 0; //how many sensor we have

uint8_t int_nu = 0; //for rotary encoder
uint8_t flag = 0;

//part of the variables are still public for debug, set
#line 278 "f:\\WaterArduino\\watering\\watering.ino"
void setup();
#line 296 "f:\\WaterArduino\\watering\\watering.ino"
void loop();
#line 323 "f:\\WaterArduino\\watering\\watering.ino"
void drawMenu();
#line 411 "f:\\WaterArduino\\watering\\watering.ino"
void drawHomePage();
#line 492 "f:\\WaterArduino\\watering\\watering.ino"
void buttonPressed();
#line 613 "f:\\WaterArduino\\watering\\watering.ino"
void readQuadrature();
#line 711 "f:\\WaterArduino\\watering\\watering.ino"
void restMenuData();
#line 278 "f:\\WaterArduino\\watering\\watering.ino"
void setup()
{
    Serial.begin(9600);
    pinMode(DHT11PIN, OUTPUT); //to send data to DHT11 for hand shaking, so using output
    pinMode(A, INPUT);         //for encoder A pin and B pin
    pinMode(B, INPUT);
    pinMode(5, INPUT); //for te switch on encoder
    //attachInterrupt(1, readQuadrature, CHANGE);
    attachInterrupt(0, readQuadrature, CHANGE);
    //attachInterrupt(5, buttonPressed, CHANGE); //switch
    u8g2.begin();
    for (int i = 0; i < 4; i++)
    { //for test
        PlantSensor sensor(arduinoPort.getServoPort(), arduinoPort.getSensorName(), arduinoPort.getHumidityPort());
        sensors.push_back(sensor);
    }
}

void loop()
{
    if (enterMenu)
        //Serial.println("yes");
        //Serial.println(currentItem);
        //Serial.println(currentMenu);
        u8g2.firstPage();
    do
    {
        if (enterMenu == false)
        {
            drawHomePage();
        }
        else
        {
            /*switch (currentMenu)
                { //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor7:adjust menu
                case 1:
                }*/
            // drawMenu();
        }

    } while (u8g2.nextPage());

    delay(1000);
}

void drawMenu()
{
    vector<char> items; //used to store how many items in a menu
    uint8_t i, h;
    u8g2_uint_t w, d;

    u8g2.setFont(u8g_font_6x13);
    u8g2.setFontRefHeightText();
    u8g2.setFontPosTop(); //以左上角为坐标原点

    h = u8g2.getAscent() - u8g2.getDescent(); //height of
    w = u8g2.getDisplayWidth();

    switch (currentMenu)
    {
    case 1:
        for (int n = 0; n < 4; n++)
        {
            items.push_back(mainMenuItem[n]);
        }
        items.push_back("Back");
        break;
    case 2:
        for (int n = 0; n < 4; n++)
        {
            items.push_back(sensors[n].getName());
        }
        items.push_back("Back");
        break;
    case 3:
        for (int n = 0; n < 4; n++)
        {
            items.push_back(sensors[n].getName());
        }
        items.push_back("Back");
        break;
    case 4:
        for (int n = 0; n < 4; n++)
        {
            items.push_back(sensors[n].getName()); //temperaly is just get names, will be change to get date
        }
        items.push_back("Back");
        break;

    case 5:
        for (int n = 0; n < 4; n++)
        {
            items.push_back(sensors[n].getName());
        }
        items.push_back("Back");
        break;

    case 6:
        for (int n = 0; n < 4; n++)
        {
            items.push_back(menuItemFor6[n]);
        }
        items.push_back("Back");
        break;
    }

    if (currentMenu == 7)
    {
        u8g2.setFont(u8g2_font_unifont_t_symbols); //set fonts
        u8g2.drawGlyph(48, 40, 0x23f5);
        u8g2.drawGlyph(88, 40, 0x23f6);
        u8g2.setFont(u8g_font_6x13);
        u8g2.setCursor(68, 40);
        u8g2.print(currentItem);
    }
    else
    {
        u8g2.setFont(u8g2_font_5x7_tr);
        for (i = 0; i < items.size(); i++)
        {
            d = (w - u8g2.getStrWidth(items[i])) / 2; //x position
            //u8g2.setDefaultForegroundColor();
            if (i == currentItem)
            {
                u8g2.drawFrame(0, i * h + 16, w, h);
                //u8g2.setDefaultBackgroundColor();
            }
            u8g2.setCursor(d, i * h + 16);
            u8g2.print(items[i]);
        }
    }
}

void drawHomePage()

{
    uint8_t i, h;
    u8g2_uint_t d;

    u8g2.setFontRefHeightText(); // Ascent will be the ascent of "A" or "1" of the current font. Descent will be the descent "g" of the current font (this is the default after startup).
    u8g2.setFontPosTop();        //set the lefttop as  (0,0)
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.drawGlyph(d, 2, 0x00f8); //signal sign
    u8g2.setFont(u8g_font_6x13);
    u8g2.drawStr(d + 50, 2, "Time");
    u8g2.drawLine(u8g2.getDisplayWidth(), 16, 0, 16);   //a horizontal line
    u8g2.drawLine(80, u8g2.getDisplayHeight(), 80, 16); //a vertical line
    int chk = DHT11.read(DHT11PIN);                     //将读取到的值赋给chk

    u8g2.setFont(u8g2_font_5x7_tr);           //6 pixels high
    h = u8g2.getAscent() - u8g2.getDescent(); //get text height
    switch (chk)
    {
    case DHTLIB_OK:
        u8g2.setCursor(90, h + 17);
        u8g2.print(DHT11.humidity);
        u8g2.drawStr(100, h + 17, "%");
        u8g2.setCursor(90, h + 37);
        u8g2.print(DHT11.temperature);
        u8g2.drawStr(100, h + 37, "oC");
        u8g2.setFont(u8g2_font_blipfest_07_tr); //5 pixels high
        u8g2.drawStr(82, 17, "Humidity");
        u8g2.drawStr(82, 37, "Temperature");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        u8g2.drawStr(85, h + 15, "Sensor");
        u8g2.drawStr(85, h + 30, "checksum");
        u8g2.drawStr(85, h + 45, "error");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        u8g2.drawStr(85, h + 15, "Sensor");
        u8g2.drawStr(85, h + 30, "Time out");
        break;
    default:
        u8g2.drawStr(85, h + 15, "Sensor");
        u8g2.drawStr(85, h + 30, "Unknown");
        u8g2.drawStr(85, h + 45, "error");
        break;
    }

    u8g2.setFont(u8g2_font_5x7_tr);
    h = u8g2.getAscent() - u8g2.getDescent();
    for (i = 0; i < sensors.size(); i++)
    {
        int x, y;
        switch (i)
        {
        case 0:
            x = 2;
            y = 17;
            break;

        case 1:
            x = 42;
            y = 17;
            break;

        case 2:
            x = 2;
            y = 37;
            break;

        case 3:
            x = 42;
            y = 37;
            break;
        }
        u8g2.setCursor(x, y);
        u8g2.print(sensors[i].getName());
        u8g2.setCursor(x, y + h);
        u8g2.print(sensors[i].getCurrentHumidity());
    }
}

void buttonPressed()
{
    //Serial.println("Pressed!");
    if (enterMenu == false) //if not enter the menu, then enter it
    {
        enterMenu = true;
        restMenuData();
    }
    else
    {
        switch (currentMenu)
        {       //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor 7:adjust menu
        case 1: //main menu
            switch (currentItem)
            {
            case 1:
                currentMenu = 2;
                currentItem = 1;
                break;
            case 2:
                currentMenu = 3;
                currentItem = 1;
                break;
            case 3:
                currentMenu = 5;
                currentItem = 1;
                break;
            case 4:
                currentItem = 1;
                enterMenu = false;
                break;
            }
            break;

        case 2: //manual watering
            if (currentItem == sensors.size())
            { //back
                currentItem = 1;
                currentMenu = 1;
            }
            else
            {
                sensors[currentItem - 1].setWater();
            }
            break;

        case 3: //record
            if (currentItem == sensors.size())
            {
                currentItem = 1;
                currentMenu = 1;
            }
            else
            {
                currentMenu = 4; //enter the record menu for each sensor
                currentItemForLastPage = currentItem;
                currentItem = 1;
            }
            break;

        case 4: //record menu for each sensor
            if (currentItem == 2)
            {
                currentItem = currentItemForLastPage;
                currentMenu = 3;
                sensors[currentItemForLastPage - 1].setRecord(false);
            }
            else
            {
                sensors[currentItemForLastPage - 1].setRecord(true);
            }
            break;

        case 5: //option
            if (currentItem == sensors.size())
            {
                currentItem = 5;
                currentMenu = 1;
            }
            else
            {
                currentMenu = 6; //enter the option menu for each sensor
                currentItemForLastPage = currentItem;
                currentItem = 1;
            }
            break;

        case 6:
            if (currentItem == 5)
            {
                currentItem = currentItemForLastPage;
                currentMenu = 5;
            }
            else
            {
                currentMenu = 7; //enter the record menu for each sensor
                currentItemForLastPage = currentItem;
            }
            break;

        case 7:
            currentMenu = 6;
            switch (currentItemForLastPage)
            {
            case 1:
                sensors[currentItemForLastPage - 1].setTempertureLowerLimit(currentItem);
                break;
            case 2:
                sensors[currentItemForLastPage - 1].setTempertureUpperLimit(currentItem);
                break;
            case 3:
                sensors[currentItemForLastPage - 1].setHumidityLowerLimit(currentItem);
                break;
            case 4:
                sensors[currentItemForLastPage - 1].setHumidityUpperLimit(currentItem);
                break;
            }
            currentItem = currentItemForLastPage;
        }
    }
}
void readQuadrature()
{
    //Serial.println("Rotated!");
    if (int_nu == 0 && digitalRead(A) == LOW)
    {

        flag = 0;
        if (digitalRead(B)){
            flag = 1;
        }
        int_nu = 1;
    }
    if (int_nu && digitalRead(A))
    {
        if (digitalRead(B) == LOW && flag == 1)
        {
            switch (currentMenu)
            {       //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor 7:adjust menu
            case 1: //main menu
                if (currentItem == 4)
                    currentItem = 4;
                else
                    currentItem++;
                break;

            case 2: //manual watering
                if (currentItem == sensors.size())
                    currentItem = (sensors.size());
                else
                    currentItem++;
                break;

            case 3: //record
                if (currentItem == sensors.size())
                    currentItem = (sensors.size());
                else
                    currentItem++;
                break;

            case 4: //record menu for each sensor
                if (currentItem == 2)
                    currentItem = 2;
                else
                    currentItem++;
                break;

            case 5: //option
                if (currentItem == sensors.size())
                    currentItem = (sensors.size());
                else
                    currentItem++;
                break;

            case 6:
                if (currentItem == 5)
                    currentItem = 5;
                else
                    currentItem++;
                break;

            case 7:
                if (currentItem == 50)
                    currentItem = 50;
                else
                    currentItem++;
                break;
            }
            currentItem++;
        }

        if (digitalRead(B) && flag == 0)
        {Serial.println("OK!");
            if (currentItem == 1)
            {
                currentItem = 1;
                 Serial.println("1!");
            }
            else{
                 Serial.println("--!");
                currentItem--;
            }
            
        }
        if (enterMenu == false) //if not enter the menu, then enter it
        {
            enterMenu = true;
            restMenuData();
        }
        int_nu = 0;
        Serial.println("CurrentItem");
        Serial.println(currentItem);
        Serial.println("CurrentMenu");
        Serial.println(currentMenu);
        Serial.println("flag");
        Serial.println(flag);
        Serial.println("");
    }
}
void restMenuData()
{
    currentMenu = 1; //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor
    currentPage = 1;
    currentItem = 1;
    currentItemForLastPage = 1;
}
