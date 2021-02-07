#include <Arduino.h>
#line 1 "f:\\WaterArduino\\watering\\watering.ino"

#include <dht11.h>
#include <U8g2lib.h>
#include "ArduinoSTL.h"
#include <string.h>
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

using namespace std;
#define DHT11PIN 5 //define the pin2 is the airTempHumidity sensor
#define A 3        //has been changed with B, if any bugs appear with encoder, check this
#define B 2
#define C 18

#define WEATHER_CODE_DAY_SUN "0" //晴（国内城市白天晴）
#define WEATHER_CODE_NIGHT_SUN "1" //晴（国内城市夜晚晴）
#define WEATHER_CODE_DAY_SUN1 "2" //晴（国外城市白天晴）
#define WEATHER_CODE_NIGHT_SUN2 "3" //晴（国外城市夜晚晴）
#define WEATHER_CODE_CLOUDY "4" //多云
#define WEATHER_CODE_DAY_PARTLY_CLOUDY "5" //白天晴间多云
#define WEATHER_CODE_NIGHT_PARTLY_CLOUDY "6" //夜晚晴间多云
#define WEATHER_CODE_DAY_MOSTLY_CLOUDY "7" //白天大部多云
#define WEATHER_CODE_NIGHT_MOSTLY_CLOUDY "8" //夜晚大部多云
#define WEATHER_CODE_OVERCAST "9" //阴
#define WEATHER_CODE_SHOWER "10" //阵雨
#define WEATHER_CODE_THUNDERSHOWER "11" //雷阵雨
#define WEATHER_CODE_THUNDERSHOWER_WITH_HAIL "12" //雷阵雨伴有冰雹
#define WEATHER_CODE_LIGHT_RAIN "13" //小雨
#define WEATHER_CODE_MODERATE_RAIN "14" //中雨
#define WEATHER_CODE_HEAVY_RAIN "15" //大雨
#define WEATHER_CODE_STORM "16" //暴雨
#define WEATHER_CODE_HEAVY_STORM "17" //大暴雨
#define WEATHER_CODE_SEVERE_STORM "18" //特大暴雨
#define WEATHER_CODE_ICE_RAIN "19" //冻雨
#define WEATHER_CODE_SLEET "20" //雨夹雪
#define WEATHER_CODE_SNOW_FLURRY "21" //阵雪
#define WEATHER_CODE_LIGHT_SNOW "22" //小雪
#define WEATHER_CODE_MODERATE_SNOW "23" //中雪
#define WEATHER_CODE_HEAVY_SNOW "24" //大雪
#define WEATHER_CODE_SNOW_STORM "25" //暴雪

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
    const int mHumidityPort[4] = {A8, A9, A10, A11};           //All ports for humidity sensors
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
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // All Boards without Reset of the Display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
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
        digitalWrite(mServoPort, LOW);
    }

    void watering()
    { // loop will keep call this function all the time
        if (mWaterFlag)
        {

            digitalWrite(mServoPort, HIGH);
            delay(5000);
            digitalWrite(mServoPort, LOW);
            Serial.println("Watered!");
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

struct UserData
{
    char city[16];        //city name
    char weather_code[4]; //code of weather
    char temp[5];         //temperature
    char days;
    char *hours = 0;
    char *mins = 0;
    char *seconds = 0;
    long innerTimeWhenUpdated = 0;
};

UserData userData;

bool enterMenu = false;   //if enter the menu
bool weatherMenu = false; // the menu that display weather

const char *mainMenuItem[3] = {"Manual watering", "Auto-Watering settings", "Sensor records"};
const char *menuItemFor3[4] = {"UpperTemputerture", "LowerTemputerture", "UpperHumidity", "LowerHumidity"};

uint8_t currentMenu = 0; //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor
uint8_t currentPage = 0;
uint8_t currentItem = 0;
uint8_t currentItemForLastPage = 0; //record which item selected in last item;

uint8_t sensorThatCurrentHave = 0; //how many sensor we have

uint8_t int_nu = 0; //for rotary encoder
uint8_t flag = 0;

long lastDebounceTime = 0;
bool debounce = true;

#line 329 "f:\\WaterArduino\\watering\\watering.ino"
void setup();
#line 348 "f:\\WaterArduino\\watering\\watering.ino"
void loop();
#line 438 "f:\\WaterArduino\\watering\\watering.ino"
void drawMenu();
#line 539 "f:\\WaterArduino\\watering\\watering.ino"
void drawHomePage();
#line 620 "f:\\WaterArduino\\watering\\watering.ino"
void drawWeatherPage();
#line 674 "f:\\WaterArduino\\watering\\watering.ino"
void drawTime();
#line 711 "f:\\WaterArduino\\watering\\watering.ino"
void showWeather();
#line 743 "f:\\WaterArduino\\watering\\watering.ino"
void drawWeather(uint8_t symbol, char *degree, char *city);
#line 761 "f:\\WaterArduino\\watering\\watering.ino"
void drawWeatherSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol);
#line 797 "f:\\WaterArduino\\watering\\watering.ino"
void buttonPressed();
#line 931 "f:\\WaterArduino\\watering\\watering.ino"
void readQuadrature();
#line 1027 "f:\\WaterArduino\\watering\\watering.ino"
void restMenuData();
#line 1035 "f:\\WaterArduino\\watering\\watering.ino"
void autoWatering();
#line 329 "f:\\WaterArduino\\watering\\watering.ino"
void setup()
{
    
    Serial.begin(9600);
    pinMode(A, INPUT); //for encoder A pin and B pin
    pinMode(B, INPUT);
    pinMode(C, INPUT_PULLUP); //for te switch on encoder
    //attachInterrupt(1, readQuadrature, CHANGE);
    attachInterrupt(0, readQuadrature, CHANGE);
    attachInterrupt(5, buttonPressed, LOW); //switch
    u8g2.begin();
    Wire.begin();
    for (int i = 0; i < 4; i++)
    { //for test
        PlantSensor sensor(arduinoPort.getServoPort(), arduinoPort.getSensorName(), arduinoPort.getHumidityPort());
        sensors.push_back(sensor);
    }
}

void loop()
{
    Wire.beginTransmission(8); /* begin with device address 8 */
    Wire.write("update");      /* sends hello string */
    Wire.endTransmission();    /* stop transmitting */

    char *receivedData = "";
    Wire.requestFrom(8, 13);   /* request & read data of size 13 from slave */

    

    while (0 < Wire.available())
    {
        receivedData = receivedData + Wire.read();
    }
    if (strlen(receivedData) == 13)
    {
        for (int i = 0; i < 4; i++)
        {
            userData.city[i] = receivedData[i]; //city
        }

        for (int i = 0; i < 2; i++)
        {
            userData.weather_code[i] = receivedData[i + 4]; //weather code
        }

        for (int i = 0; i < 2; i++)
        {
            userData.temp[i] = receivedData[i + 6]; // temperatuare
        }

        userData.days = receivedData[8]; //day

        for (int i = 0; i < 2; i++)
        {
            userData.hours[i] = receivedData[i + 8]; //hours
        }

        for (int i = 0; i < 2; i++)
        {
            userData.mins[i] = receivedData[i + 10]; //mins
        }

        for (int i = 0; i < 2; i++)
        {
            userData.seconds[i] = receivedData[i + 12]; //seconds
        }
        userData.innerTimeWhenUpdated = millis();
    }

    Serial.print("receivedData: ");
    Serial.println(receivedData);
    Serial.print("Time: ");
    //Serial.println(userData.days);
    Serial.print("temp: ");
    //Serial.println(userData.temp);
    Serial.println("Ok");

    if (lastDebounceTime > 200)
    {
        debounce = true; //debounce
    }

    for (int i = 0; i < 4; i++)
    {
        sensors[i].watering();
    }

    u8g2.clearBuffer();
    if (enterMenu == false)
    {
        if(weatherMenu == false){
            drawHomePage();
        }
        else
        {
            drawWeatherPage();
        }
    }
    else
    {
        drawMenu();
    }

    u8g2.sendBuffer();

    delay(1000);
}

void drawMenu()
{
    vector<char *> items; //used to store how many items in a menu
    uint8_t i, h;
    u8g2_uint_t w, d;

    u8g2.setFont(u8g_font_6x13);
    u8g2.setFontRefHeightText();
    u8g2.setFontPosTop();                             //以左上角为坐标原点
    u8g2.drawLine(u8g2.getDisplayWidth(), 15, 0, 15); //a horizontal line
    h = u8g2.getAscent() - u8g2.getDescent();         //height of fonts
    w = u8g2.getDisplayWidth();

    switch (currentMenu)
    {
    case 0: //main
        for (int n = 0; n < 3; n++)
        {
            items.push_back(mainMenuItem[n]);
        }
        items.push_back("Back");
        u8g2.drawStr((u8g2.getDisplayWidth() / 2) - ((u8g2.getStrWidth("Main Menu") / 2)), 1, "Main Menu");
        break;

    case 1: //manual
        for (int n = 0; n < sensors.size(); n++)
        {
            items.push_back(sensors[n].getName());
        }
        items.push_back("Back");
        u8g2.drawStr((u8g2.getDisplayWidth() / 2) - ((u8g2.getStrWidth("Manual Watering") / 2)), 1, "Manual Watering");
        break;

    case 2: //settings
        for (int n = 0; n < sensors.size(); n++)
        {
            items.push_back(sensors[n].getName());
        }
        items.push_back("Back");
        u8g2.drawStr((u8g2.getDisplayWidth() / 2) - ((u8g2.getStrWidth("Settings") / 2)), 1, "Settings");
        break;

    case 3: //settings for each
        for (int n = 0; n < 4; n++)
        {
            items.push_back(menuItemFor3[n]); //temperaly is just get names, will be change to get date
        }
        items.push_back("Back");
        u8g2.setCursor((u8g2.getDisplayWidth() / 2) - ((u8g2.getStrWidth(sensors[currentItemForLastPage].getName()) / 2)), 1);
        u8g2.print(sensors[currentItemForLastPage].getName());
        break;

    case 4: //records
        for (int n = 0; n < sensors.size(); n++)
        {
            items.push_back(sensors[n].getName());
        }
        items.push_back("Back");
        u8g2.drawStr((u8g2.getDisplayWidth() / 2) - ((u8g2.getStrWidth("Records") / 2)), 1, "Records");
        break;

    case 5: //records for each
        for (int n = 0; n < sensors.size(); n++)
        {
            items.push_back("menuItemFor5[n]");
        }
        items.push_back("Back");
        u8g2.setCursor((u8g2.getDisplayWidth() / 2) - ((u8g2.getStrWidth(sensors[currentItemForLastPage].getName()) / 2)), 1);
        u8g2.print(sensors[currentItemForLastPage].getName());
        break;
    }
    if (currentMenu == 6)
    {
        u8g2.setCursor((u8g2.getDisplayWidth() / 2) - ((u8g2.getStrWidth(menuItemFor3[currentItemForLastPage]) / 2)), 1);
        u8g2.print(menuItemFor3[currentItemForLastPage]);
        u8g2.drawStr((u8g2.getDisplayWidth() / 2) - ((u8g2.getStrWidth("Twist to adjust") / 2)), 16, "Twist to adjust");
        u8g2.setFont(u8g2_font_unifont_t_symbols); //set fonts
        u8g2.drawGlyph((u8g2.getDisplayWidth() / 2) - 20, 40, 0x23f4);
        u8g2.drawGlyph((u8g2.getDisplayWidth() / 2) + u8g2.getStrWidth("0"), 40, 0x23f5);
        u8g2.setFont(u8g_font_6x13);
        u8g2.setCursor(((u8g2.getDisplayWidth() / 2) - u8g2.getStrWidth("0") / 2), 40);
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
                u8g2.drawFrame(0, i * h + 17, w, h);
                //u8g2.setDefaultBackgroundColor();
            }
            u8g2.setCursor(d, i * h + 18);
            u8g2.print(items[i]);
        }
    }
}

void drawHomePage()
{
    uint8_t i, h;

    
    Serial.println("Home");
    u8g2.setFontRefHeightText(); // Ascent will be the ascent of "A" or "1" of the current font. Descent will be the descent "g" of the current font (this is the default after startup).
    u8g2.setFontPosTop();        //set the lefttop as  (0,0)
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.drawGlyph(0, 2, 0x00f8); //signal sign
    u8g2.setFont(u8g_font_6x13);
    drawTime();
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
    } //Serial.println("Finish!");

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

void drawWeatherPage()
{
    Serial.println("Weather!");
    uint8_t i, h;
    u8g2_uint_t d;

    u8g2.setFontRefHeightText(); // Ascent will be the ascent of "A" or "1" of the current font. Descent will be the descent "g" of the current font (this is the default after startup).
    u8g2.setFontPosTop();        //set the lefttop as  (0,0)
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.drawGlyph(d, 2, 0x00f8); //signal sign
    u8g2.setFont(u8g_font_6x13);

    drawTime();
    showWeather();

    u8g2.drawLine(u8g2.getDisplayWidth(), 16, 0, 16);   //a horizontal line
    u8g2.drawLine(80, u8g2.getDisplayHeight(), 80, 16); //a vertical line
    int chk = DHT11.read(DHT11PIN);                     //assign the value read to the dht11

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
    } //Serial.println("Finish!");

    u8g2.setFont(u8g2_font_5x7_tr);
    h = u8g2.getAscent() - u8g2.getDescent();
}

void drawTime()
{
    u8g2_uint_t d = 2;
    u8g2.setCursor(d + 40, 2);
    u8g2.print(userData.hours);
    u8g2.drawStr(d + 48, 2, ":");
    u8g2.setCursor(d + 50, 2);
    u8g2.print(userData.mins);
    u8g2.drawStr(d + 56, 2, ":");
    u8g2.setCursor(d + 58, 2);
    u8g2.print(userData.seconds);
    switch (userData.days)
    {
    case '1':
        u8g2.drawStr(d + 60, 2, "Mon.");
        break;
    case '2':
        u8g2.drawStr(d + 60, 2, "Tues.");
        break;
    case '3':
        u8g2.drawStr(d + 60, 2, "Wed.");
        break;
    case '4':
        u8g2.drawStr(d + 60, 2, "Thur.");
        break;
    case '5':
        u8g2.drawStr(d + 60, 2, "Fri.");
        break;
    case '6':
        u8g2.drawStr(d + 60, 2, "Sat.");
        break;
    case '0':
        u8g2.drawStr(d + 60, 2, "Sun.");
        break;
    }
}

void showWeather()
{
    if (strcmp(userData.weather_code, WEATHER_CODE_DAY_SUN) == 0 || strcmp(userData.weather_code, WEATHER_CODE_DAY_SUN1) == 0)
    {
        drawWeather(0, userData.temp, userData.city);
    }
    else if (strcmp(userData.weather_code, WEATHER_CODE_NIGHT_SUN) == 0 || strcmp(userData.weather_code, WEATHER_CODE_NIGHT_SUN2) == 0)
    {
        drawWeather(1, userData.temp, userData.city);
    }
    else if (strcmp(userData.weather_code, WEATHER_CODE_DAY_PARTLY_CLOUDY) == 0 || strcmp(userData.weather_code, WEATHER_CODE_NIGHT_PARTLY_CLOUDY) == 0)
    {
        drawWeather(2, userData.temp, userData.city);
    }
    else if (strcmp(userData.weather_code, WEATHER_CODE_CLOUDY) == 0 || strcmp(userData.weather_code, WEATHER_CODE_DAY_MOSTLY_CLOUDY) == 0 || strcmp(userData.weather_code, WEATHER_CODE_NIGHT_MOSTLY_CLOUDY) == 0 || strcmp(userData.weather_code, WEATHER_CODE_OVERCAST) == 0)
    {
        drawWeather(3, userData.temp, userData.city);
    }
    else if (strcmp(userData.weather_code, WEATHER_CODE_SHOWER) == 0 || strcmp(userData.weather_code, WEATHER_CODE_LIGHT_RAIN) == 0 || strcmp(userData.weather_code, WEATHER_CODE_MODERATE_RAIN) == 0 || strcmp(userData.weather_code, WEATHER_CODE_HEAVY_RAIN) == 0 || strcmp(userData.weather_code, WEATHER_CODE_STORM) == 0 || strcmp(userData.weather_code, WEATHER_CODE_HEAVY_STORM) == 0 || strcmp(userData.weather_code, WEATHER_CODE_SEVERE_STORM) == 0)
    {
        drawWeather(4, userData.temp, userData.city);
    }
    else if (strcmp(userData.weather_code, WEATHER_CODE_THUNDERSHOWER) == 0 || strcmp(userData.weather_code, WEATHER_CODE_THUNDERSHOWER_WITH_HAIL) == 0)
    {
        drawWeather(5, userData.temp, userData.city);
    }
    else
    {
        drawWeather(6, userData.temp, userData.city);
    }
}

void drawWeather(uint8_t symbol, char *degree, char *city)
{
    
    drawWeatherSymbol(0, 48, symbol);
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.setCursor(51, 42);
    u8g2.print(degree);
    u8g2.print("oC");
    //u8g2.setFont(u8g2_font_unifont_t_chinese3);

    u8g2_uint_t strWidth = u8g2.getUTF8Width(city);
    u8g2_uint_t displayWidth = u8g2.getDisplayWidth();

    u8g2.setCursor(displayWidth - strWidth - 5, 60);
    u8g2.print(city);

}

void drawWeatherSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol)

{
    // fonts used:
    // u8g2_font_open_iconic_embedded_6x_t
    // u8g2_font_open_iconic_weather_6x_t
    // encoding values, see: https://github.com/olikraus/u8g2/wiki/fntgrpiconic
    switch (symbol)
    {
    case 0: //太阳
        u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
        u8g2.drawGlyph(x, y, 69);
        break;
    case 1: //太阳
        u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
        u8g2.drawGlyph(x, y, 66);
        break;
    case 2: //晴间多云
        u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
        u8g2.drawGlyph(x, y, 65);
        break;
    case 3: //多云
        u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
        u8g2.drawGlyph(x, y, 64);
        break;
    case 4: //下雨
        u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
        u8g2.drawGlyph(x, y, 67);
        break;
    case 5: //打雷
        u8g2.setFont(u8g2_font_open_iconic_embedded_6x_t);
        u8g2.drawGlyph(x, y, 67);
        break;
    }
}

void buttonPressed()
{
    if (debounce)
    {
        lastDebounceTime = millis();

        Serial.println("Pressed!");
        if (enterMenu == false) //if not enter the menu, then change the data
        {
            if(weatherMenu) {
                weatherMenu = false;
            }else{
                weatherMenu = true;
            }
        }
        else
        {
            switch (currentMenu)
            {       //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor 7:adjust menu
            case 0: //main menu
                switch (currentItem)
                {
                case 0:
                    currentMenu = 1;
                    currentItem = 0;
                    break;
                case 1:
                    currentMenu = 2;
                    currentItem = 0;
                    break;
                case 2:
                    currentMenu = 4;
                    currentItem = 0;
                    break;
                case 3:
                    currentItem = 0;
                    enterMenu = false;
                    Serial.print("exit!");
                    break;
                }
                break;

            case 1: //manual watering
                if (currentItem == sensors.size())
                { //back
                    currentItem = 0;
                    currentMenu = 0;
                    Serial.print("back!");
                }
                else
                {
                    sensors[currentItem].setWater();
                }
                break;

            case 2: //settings
                if (currentItem == sensors.size())
                {
                    currentItem = 1;
                    currentMenu = 0;
                }
                else
                {
                    currentMenu = 3; //enter settings menu for each sensor
                    currentItemForLastPage = currentItem;
                    currentItem = 0;
                }
                break;

            case 3:                   //setting menu for each sensor
                if (currentItem == 4) //back
                {
                    currentItem = currentItemForLastPage;
                    currentMenu = 2;
                }
                else
                {
                    currentMenu = 6; //enter the settings menu for each sensor
                    currentItemForLastPage = currentItem;
                    currentItem = 0;
                }
                break;

            case 4: //records
                if (currentItem == sensors.size())
                {
                    currentItem = 5;
                    currentMenu = 0;
                }
                else
                {
                    currentMenu = 5; //enter the records menu for each sensor
                    currentItemForLastPage = currentItem;
                    currentItem = 0;
                }
                break;

            case 5: //records menu for each sensor
                if (currentItem == 0)
                {
                    currentItem = currentItemForLastPage;
                    currentMenu = 4;
                    sensors[currentItemForLastPage].setRecord(false);
                }
                else
                {
                    sensors[currentItemForLastPage].setRecord(true);
                }
                break;

            case 6:
                currentMenu = 3;
                switch (currentItemForLastPage)
                {
                case 1:
                    sensors[currentItemForLastPage].setTempertureLowerLimit(currentItem);
                    break;
                case 2:
                    sensors[currentItemForLastPage].setTempertureUpperLimit(currentItem);
                    break;
                case 3:
                    sensors[currentItemForLastPage].setHumidityLowerLimit(currentItem);
                    break;
                case 4:
                    sensors[currentItemForLastPage].setHumidityUpperLimit(currentItem);
                    break;
                }
                currentItem = currentItemForLastPage;
            }
        }
        debounce = false;
    }
}

void readQuadrature()
{
    if (int_nu == 0 && digitalRead(A) == LOW)
    {

        flag = 0;
        if (digitalRead(B))
        {
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
            case 0: //main menu
                if (currentItem == 3)
                    currentItem = 3;
                else
                    currentItem++;
                break;

            case 1: //manual watering
                if (currentItem == sensors.size())
                    currentItem = (sensors.size());
                else
                    currentItem++;
                break;

            case 2: //settings
                if (currentItem == sensors.size())
                    currentItem = (sensors.size());
                else
                    currentItem++;
                break;

            case 3: //settings menu for each sensor
                if (currentItem == 4)
                    currentItem = 4;
                else
                    currentItem++;
                break;

            case 4: //records

                if (currentItem == sensors.size())
                    currentItem = (sensors.size());
                else
                    currentItem++;
                break;

            case 5: //records for each sensors
                if (currentItem == 0)
                    currentItem = 0;
                else
                    currentItem++;
                break;

            case 6:
                if (currentItem == 50)
                    currentItem = 50;
                else
                    currentItem++;
                break;
            }
        }

        if (digitalRead(B) && flag == 0)
        {
            if (currentItem == 0)
            {
                currentItem = 0;
            }
            else
            {
                currentItem--;
            }
        }
        if (enterMenu == false) //if not enter the menu, then enter it
        {
            enterMenu = true;
            restMenuData();
        }
        int_nu = 0;

        Serial.print("Item: ");
        Serial.println(currentItem);
        Serial.print("Menu: ");
        Serial.println(currentMenu);
        Serial.println("--------");
    }
}

void restMenuData()
{
    currentMenu = 0; //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor
    currentPage = 0;
    currentItem = 0;
    currentItemForLastPage = 0;
}

void autoWatering()
{
    for (int i = 0; i < 4; i++)
    {
        int chk = DHT11.read(DHT11PIN);
        if (chk == "DHTLIB_OK")
        {
            if ((sensors[i].getTempertureLowerLimit() < DHT11.temperature) && (sensors[i].getTempertureUpperLimit() > DHT11.temperature) && (sensors[i].getHumidityLowerLimit() < DHT11.humidity) && (sensors[i].getHumidityUpperLimit() > DHT11.humidity))
            {
                sensors[i].setWater();
            }
        }
    }
}
