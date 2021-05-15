# 1 "f:\\WaterArduino\\watering\\watering.ino"

# 3 "f:\\WaterArduino\\watering\\watering.ino" 2
# 4 "f:\\WaterArduino\\watering\\watering.ino" 2
# 5 "f:\\WaterArduino\\watering\\watering.ino" 2
# 6 "f:\\WaterArduino\\watering\\watering.ino" 2
# 7 "f:\\WaterArduino\\watering\\watering.ino" 2


# 10 "f:\\WaterArduino\\watering\\watering.ino" 2


using namespace std;
# 45 "f:\\WaterArduino\\watering\\watering.ino"
union limits
{
    int realLimits;
    byte storeFlag[2];
}; //using a union so that we can store int in eeprom

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
    const int mHumidityPort[4] = {A8, A9, A10, A11}; //All ports for humidity sensors
    const int mServoPort[4] = {8, 9, 10, 11}; //All ports for servo motors
    const char *mSensors[4] = {"Sensor1", "Sensor2", "Sensor3", "Sensor4"};
};

AvaliablePort::AvaliablePort()
{ //not sure if it work, (pinmode outside of the main function)
    for (int i = 0; i < 4; i++)
    {
        pinMode(mHumidityPort[i], 0x0);
        pinMode(mServoPort[i], 0x1);
    }
}

AvaliablePort arduinoPort; //instantiating it

//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // All Boards without Reset of the Display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2((&u8g2_cb_r0), /* reset=*/255);
class PlantSensor
{
public:
    void setTempertureLowerLimit(int lowerLimit)
    {
        mTemperatureInterval[0].realLimits = lowerLimit;
        EEPROM.update(mEeprom, mTemperatureInterval[0].storeFlag[0]);
        EEPROM.update(mEeprom + 1, mTemperatureInterval[0].storeFlag[1]);
    }

    void setTempertureUpperLimit(int upperLimit)
    {
        mTemperatureInterval[1].realLimits = upperLimit;
        EEPROM.update(mEeprom + 2, mTemperatureInterval[1].storeFlag[0]);
        EEPROM.update(mEeprom + 3, mTemperatureInterval[1].storeFlag[1]);
    }

    void setHumidityLowerLimit(int lowerLimit)
    {
        mHumidityInterval[0].realLimits = lowerLimit;
        EEPROM.update(mEeprom + 4, mHumidityInterval[0].storeFlag[0]);
        EEPROM.update(mEeprom + 5, mHumidityInterval[0].storeFlag[1]);
    }

    void setHumidityUpperLimit(int upperLimit)
    {
        mHumidityInterval[1].realLimits = upperLimit;
        EEPROM.update(mEeprom + 6, mHumidityInterval[1].storeFlag[0]);
        EEPROM.update(mEeprom + 7, mHumidityInterval[1].storeFlag[1]);
    }

    void setCurrentHumidity()
    {
        if (mWaterFlag)
        {
            long temps = analogRead(mSensorPort);
            mCurrentHumidity = (100 - ((temps * 100) / 614));
        }
    }

    void readSavedSettings()
    {
        mTemperatureInterval[0].storeFlag[0] = EEPROM.read(mEeprom);
        mTemperatureInterval[0].storeFlag[1] = EEPROM.read(mEeprom + 1);
        mTemperatureInterval[1].storeFlag[0] = EEPROM.read(mEeprom + 2);
        mTemperatureInterval[1].storeFlag[1] = EEPROM.read(mEeprom + 3);
        mHumidityInterval[0].storeFlag[0] = EEPROM.read(mEeprom + 4);
        mHumidityInterval[0].storeFlag[1] = EEPROM.read(mEeprom + 5);
        mHumidityInterval[1].storeFlag[0] = EEPROM.read(mEeprom + 6);
        mHumidityInterval[1].storeFlag[1] = EEPROM.read(mEeprom + 7);
    }

    /*void ResetDaysSinceLastWatering()

    {

        mDaysSinceLastWatering = 0;

    }*/
# 196 "f:\\WaterArduino\\watering\\watering.ino"
    /*void DaysPlusOne()

    {

        mDaysSinceLastWatering++;

    }*/
# 201 "f:\\WaterArduino\\watering\\watering.ino"
    void setSensorPort(int port)
    {
        mSensorPort = port;
        pinMode(mSensorPort, 0x0);
    }

    void setServoPort(int port)
    {
        mServoPort = port;
        pinMode(mServoPort, 0x1);
        digitalWrite(mServoPort, 0x0);
    }

    void watering()
    { // loop will keep call this function all the time
        if (mWaterFlag)
        {

            digitalWrite(mServoPort, 0x1);
            if (lastWater == 0)
            {
                lastWater = millis();
            }
            if ((millis() - lastWater) > 3000)
            {
                digitalWrite(mServoPort, 0x0);
                //Serial.println("Watered!");
                mWaterFlag = false;
                lastWater = 0;
            }
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
# 257 "f:\\WaterArduino\\watering\\watering.ino"
    int getTempertureLowerLimit()
    {
        mTemperatureInterval[0].storeFlag[0] = EEPROM.read(mEeprom);
        mTemperatureInterval[0].storeFlag[1] = EEPROM.read(mEeprom + 1);
        return mTemperatureInterval[0].realLimits;
    }

    int getTempertureUpperLimit()
    {
        mTemperatureInterval[1].storeFlag[0] = EEPROM.read(mEeprom + 2);
        mTemperatureInterval[1].storeFlag[1] = EEPROM.read(mEeprom + 3);
        return mTemperatureInterval[1].realLimits;
    }

    int getHumidityLowerLimit()
    {
        mHumidityInterval[0].storeFlag[0] = EEPROM.read(mEeprom + 4);
        mHumidityInterval[0].storeFlag[1] = EEPROM.read(mEeprom + 5);
        return mHumidityInterval[0].realLimits;
    }

    int getHumidityUpperLimit()
    {
        mHumidityInterval[1].storeFlag[0] = EEPROM.read(mEeprom + 6);
        mHumidityInterval[1].storeFlag[1] = EEPROM.read(mEeprom + 7);
        return mHumidityInterval[1].realLimits;
    }

    int getCurrentHumidity()
    {
        return mCurrentHumidity;
    }
    /*int setCurrentTemperture(int temp)

    {

        return mCurrentTemperture.realLimits;

    }*/
# 294 "f:\\WaterArduino\\watering\\watering.ino"
    /*int getDaysSinceLastWatering()

    {

        return mDaysSinceLastWatering;

    }*/
# 299 "f:\\WaterArduino\\watering\\watering.ino"
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
    limits mTemperatureInterval[2], mHumidityInterval[2];
    int mCurrentTemperture, mCurrentHumidity, mEeprom; //mDaysSinceLastWatering;
    char *mName;
    int mSensorPort, mServoPort;
    volatile bool mWaterFlag = false;
    volatile bool mRecordFlag = false;
    volatile bool mOptionFlag = false;
    long lastWater = 0;
};

PlantSensor::PlantSensor(int port2, char *name, int port1) //name means which plant it is
{
    //first is sensor port and second one is servo
    mSensorPort = port1;
    mServoPort = port2;
    mEeprom = (8 * (mSensorPort + mServoPort) - 272);
    mCurrentTemperture = 0;
    mCurrentHumidity = 0;
    //mDaysSinceLastWatering = 0;
    mName = name;
}

PlantSensor::~PlantSensor()
{
    //arduinoPort.freeThePort(mSensorPort); //Make the port be avaliable
}

vector<PlantSensor> sensors;

String city; //city name
String weather_code = "00"; //code of weather
String temp = "0"; //temperature
String days = "0";
String hours = "00";
String mins = "00";

//bool lcdBackLight = true; //back light
bool enterMenu = false; //if enter the menu
bool weatherMenu = false; // the menu that display weather
bool signalOrWeather = false; // decide what data to request

const char *mainMenuItem[3] = {"Manual watering", "Auto-Watering settings", "Sensor records"};
const char *menuItemFor3[4] = {"UpperTemputerture", "LowerTemputerture", "UpperHumidity", "LowerHumidity"};

uint8_t currentMenu = 0; //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor
uint8_t currentPage = 0;
uint8_t currentItem = 0;
uint8_t currentItemForLastPage = 0; //recording which item was selected in last item;
uint8_t sensorID = 0;

uint8_t sensorThatCurrentHave = 0; //how many sensors we have

uint8_t int_nu = 0; //for rotary encoder
uint8_t flag = 0;

long lastDebounceTime = 0;
long menuStandbyTime = 0;
//long lcdStandbyTime = 0;

bool update = false;
bool debounce = true;
bool ifConnected = false; // see if two device are connected

String signalStrength = "00%";
uint8_t connectionCountdown = 0;
String receivedData = "";

void setup()
{

    Serial.begin(9600);
    Serial3.begin(9600);
    pinMode(3 /*has been changed with B, if any bugs appear with encoder, check this*/, 0x0); //for encoder A pin and B pin
    pinMode(2, 0x0);
    pinMode(18, 0x2); //for te switch on encoder
    //attachInterrupt(1, readQuadrature, CHANGE);
    attachInterrupt(0, readQuadrature, 1);
    attachInterrupt(5, buttonPressed, 0x0); //switch
    u8g2.begin();
    readSettings();
    //Wire.begin();
    for (int i = 0; i < 4; i++)
    { //for test
        PlantSensor sensor(arduinoPort.getServoPort(), arduinoPort.getSensorName(), arduinoPort.getHumidityPort());
        sensors.push_back(sensor);
    }
}

void loop()
{
    if ((millis() - menuStandbyTime) > 10000)
    {
        enterMenu = false;
    }
    //if(((millis() - lcdStandbyTime) >30000)&&(!enterMenu)){
    // u8g2.setPowerSave(0);
    //}
    autoWatering();
    connection();
    readMoisture();

    if (!update)
    {
        String YesOrNo = "0";

        if (Serial3.available() == 0)
        {
            Serial3.write("ready?");
            delay(10);
            //Serial.println("Print!");
        }

        bool flag = true; // make sure the next Serial only read one bit in one time

        while (Serial3.available() > 0)
        {
            delay(2);
            receivedData = receivedData + (char)Serial3.read();
        }
        //Serial.print("[");
        //Serial.print(receivedData);
        //Serial.println("]");
        YesOrNo = receivedData.substring(0, 1);

        if ((receivedData.length() == 3) || (receivedData.length() == 4))
        {
            signalStrength = receivedData.substring(1);

            connectionCountdown = 5;
        }
        else if (connectionCountdown > 0)
        {
            connectionCountdown--;
        }

        //Serial.print("YesOrNo ");
        //Serial.print(YesOrNo);
        //Serial.print(" YesOrNo");

        if (YesOrNo == "1")
        {

            //Serial.println("True");
            update = true;
        }
        else if ((YesOrNo == "") && (connectionCountdown > 0))
        {
            connectionCountdown--;
        }

        //delay(1000);

        /*while(Serial3.available() > 0)

        {

            Serial3.read();

        }*/
# 474 "f:\\WaterArduino\\watering\\watering.ino"
    }
    else if (update)
    {
        //Serial.print("updated!");
        /*Wire.beginTransmission(8);

        Wire.write("update");

        Wire.endTransmission();

        Wire.requestFrom(8, 11);*/
# 482 "f:\\WaterArduino\\watering\\watering.ino"
        Serial3.print("update");
        delay(50);
        while (Serial3.available() > 0)
        {
            delay(2);
            receivedData = receivedData + (char)Serial3.read();
        }
        //Serial.println(receivedData.length());
        //Serial.print("receivedData: ");
        //Serial.println(receivedData);

        char *tempData = receivedData.c_str();
        if (receivedData.length() == 19)
        { // sometimes the string will receive 3 more chars that not belongs here, we should delete them
            receivedData = receivedData.substring(0, 4);
        }
        if ((receivedData.length() == 16) || (receivedData.length() == 17))
        { //Serial.println(receivedData.substring(0, 4).c_str());

            city = receivedData.substring(0, 4); //city
            //Serial.println(city);

            weather_code = receivedData.substring(4, 6); //weather code

            temp = receivedData.substring(6, 8); // temperatuare

            days = receivedData.substring(8, 9); //day

            hours = receivedData.substring(9, 11); //hours

            mins = receivedData.substring(11, 13); //mins

            signalStrength = receivedData.substring(13);

            //innerTimeWhenUpdated = millis();
        }

        update = false;

        //Serial.print(receivedData);
        if ((signalStrength == "00%") && (connectionCountdown > 0))
        {
            connectionCountdown--;
        }
        else
        {
            connectionCountdown = 5;
        }
    }
    receivedData = "";
    /*

    Wire.beginTransmission(8);

    Wire.write("signal");

    Wire.endTransmission();

    Wire.requestFrom(8, 3);

    */
# 539 "f:\\WaterArduino\\watering\\watering.ino"
    /*Serial.print("receivedData: ");

    Serial.println(receivedData);

    Serial.println("Time: ");

    Serial.println(days);

    Serial.print("temp: ");

    Serial.println(weather_code);*/
# 545 "f:\\WaterArduino\\watering\\watering.ino"
    receivedData = "";

    if ((millis() - lastDebounceTime) > 100)
    {
        debounce = true; //debounce
    }

    for (int i = 0; i < 4; i++) //Every loop will run this function to water the system
    {
        sensors[i].watering();
    }

    u8g2.clearBuffer();

    if (enterMenu == false)
    {
        if (weatherMenu == false)
        {
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

    //Serial.print("\n Next Round \n");
}

void drawMenu()
{
    vector<char *> items; //used to store how many items in a menu
    uint8_t i, h;
    u8g2_uint_t w, d;

    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.setFontRefHeightText();
    u8g2.setFontPosTop(); //以左上角为坐标原点
    u8g2.drawLine(u8g2.getDisplayWidth(), 15, 0, 15); //a horizontal line
    h = u8g2.getAscent() - u8g2.getDescent(); //height of fonts
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
        u8g2.drawGlyph((u8g2.getDisplayWidth() / 2) - u8g2.getStrWidth("100"), 40, 0x23f4);
        u8g2.drawGlyph((u8g2.getDisplayWidth() / 2) + u8g2.getStrWidth("10"), 40, 0x23f5);
        u8g2.setFont(u8g2_font_6x13_tf);
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

    //Serial.println("Home");
    u8g2.setFontRefHeightText(); // Ascent will be the ascent of "A" or "1" of the current font. Descent will be the descent "g" of the current font (this is the default after startup).
    u8g2.setFontPosTop(); //set the lefttop as  (0,0)
    u8g2.setFont(u8g2_font_open_iconic_www_1x_t);
    u8g2.drawGlyph(0, 2, 72); //signal sign
    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.setCursor(10, 2);
    u8g2.print(signalStrength);

    drawTime();
    drawConnectionIcon();

    u8g2.drawLine(u8g2.getDisplayWidth(), 16, 0, 16); //a horizontal line
    u8g2.drawLine(80, u8g2.getDisplayHeight(), 80, 16); //a vertical line
    int chk = DHT11.read(5 /*define the pin2 is the airTempHumidity sensor*/); //将读取到的值赋给chk

    u8g2.setFont(u8g2_font_5x7_tr); //6 pixels high
    h = u8g2.getAscent() - u8g2.getDescent(); //get text height
    switch (chk)
    {
    case 0:
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
    case -1:
        u8g2.drawStr(85, h + 15, "Sensor");
        u8g2.drawStr(85, h + 30, "checksum");
        u8g2.drawStr(85, h + 45, "error");
        break;
    case -2:
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
    //Serial.println("Weather!");
    uint8_t i, h;

    u8g2.setFontRefHeightText(); // Ascent will be the ascent of "A" or "1" of the current font. Descent will be the descent "g" of the current font (this is the default after startup).
    u8g2.setFontPosTop(); //set the lefttop as  (0,0)
    u8g2.setFont(u8g2_font_open_iconic_www_1x_t);
    u8g2.drawGlyph(0, 2, 72); //signal sign
    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.setCursor(10, 2);
    u8g2.print(signalStrength);

    drawTime();
    showWeather();
    drawConnectionIcon();

    u8g2.drawLine(u8g2.getDisplayWidth(), 16, 0, 16); //a horizontal line
    u8g2.drawLine(80, u8g2.getDisplayHeight(), 80, 16); //a vertical line
    int chk = DHT11.read(5 /*define the pin2 is the airTempHumidity sensor*/); //assign the value read to the dht11

    u8g2.setFont(u8g2_font_5x7_tr); //6 pixels high
    h = u8g2.getAscent() - u8g2.getDescent(); //get text height
    switch (chk)
    {
    case 0:
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
    case -1:
        u8g2.drawStr(85, h + 15, "Sensor");
        u8g2.drawStr(85, h + 30, "checksum");
        u8g2.drawStr(85, h + 45, "error");
        break;
    case -2:
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
    u8g2.setCursor(d + 35, 2);
    u8g2.print(hours);
    u8g2.drawStr(d + 47, 2, ":");
    u8g2.setCursor(d + 53, 2);
    u8g2.print(mins);

    if (days == "1")
    {
        u8g2.drawStr(d + 70, 2, "Mon.");
    }
    else if (days == "2")
    {
        u8g2.drawStr(d + 70, 2, "Tues.");
    }
    else if (days == "3")
    {
        u8g2.drawStr(d + 70, 2, "Wed.");
    }
    else if (days == "4")
    {
        u8g2.drawStr(d + 70, 2, "Thur.");
    }
    else if (days == "5")
    {
        u8g2.drawStr(d + 70, 2, "Fri.");
    }
    else if (days == "6")
    {
        u8g2.drawStr(d + 70, 2, "Sat.");
    }
    else if (days == "0")
    {
        u8g2.drawStr(d + 70, 2, "Sun.");
    }
}

void showWeather()
{
    if (weather_code == "00" /*晴（国内城市白天晴）*/ || weather_code == "02" /*晴（国外城市白天晴）*/)
    {
        drawWeather(0, temp.c_str(), city.c_str());
    }
    else if (weather_code == "01" /*晴（国内城市夜晚晴）*/ || weather_code == "03" /*晴（国外城市夜晚晴）*/)
    {
        drawWeather(1, temp.c_str(), city.c_str());
    }
    else if (weather_code == "05" /*白天晴间多云*/ || weather_code == "06" /*夜晚晴间多云*/)
    {
        drawWeather(2, temp.c_str(), city.c_str());
    }
    else if (weather_code == "04" /*多云*/ || weather_code == "07" /*白天大部多云*/ || weather_code == "08" /*夜晚大部多云*/ || weather_code == "09" /*阴*/)
    {
        drawWeather(3, temp.c_str(), city.c_str());
    }
    else if (weather_code == "10" /*阵雨*/ || weather_code == "13" /*小雨*/ || weather_code == "14" /*中雨*/ || weather_code == "15" /*大雨*/ || weather_code == "16" /*暴雨*/ || weather_code == "17" /*大暴雨*/ || weather_code == "18" /*特大暴雨*/)
    {
        drawWeather(4, temp.c_str(), city.c_str());
    }
    else if (weather_code == "11" /*雷阵雨*/ || weather_code == "12" /*雷阵雨伴有冰雹*/)
    {
        drawWeather(5, temp.c_str(), city.c_str());
    }
    else
    {
        drawWeather(6, temp.c_str(), city.c_str());
    }
}

void drawWeather(uint8_t symbol, char *degree, char *city)
{

    //u8g2_font_5x7_tr
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(2, 17, "Temp:");
    u8g2.setCursor(27, 17);
    u8g2.print(degree);
    u8g2.setCursor(37, 17);
    u8g2.print("oC");

    u8g2.drawStr(2, 27, "City:");
    u8g2_uint_t strWidth = u8g2.getUTF8Width(city);
    u8g2_uint_t displayWidth = u8g2.getDisplayWidth();
    u8g2.setCursor(2, 37);
    u8g2.print(city);
    drawWeatherSymbol(47, 30, symbol);
}

void drawWeatherSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol)

{
    switch (symbol)
    {
    case 0: //太阳
        u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
        u8g2.drawGlyph(x, y, 69);
        break;
    case 1: //太阳
        u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
        u8g2.drawGlyph(x, y, 66);
        break;
    case 2: //晴间多云
        u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
        u8g2.drawGlyph(x, y, 65);
        break;
    case 3: //多云
        u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
        u8g2.drawGlyph(x, y, 64);
        break;
    case 4: //下雨
        u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
        u8g2.drawGlyph(x, y, 67);
        break;
    case 5: //打雷
        u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
        u8g2.drawGlyph(x, y, 67);
        break;
    }
}

void drawConnectionIcon()
{
    if (ifConnected)
    {
        u8g2.setFont(u8g2_font_open_iconic_www_1x_t);
        u8g2.drawGlyph(112, 3, 79);
    }
    else
    {
        u8g2.setFont(u8g2_font_open_iconic_www_1x_t);
        u8g2.drawGlyph(112, 3, 69);
    }
}

void buttonPressed()
{
    if (debounce)
    {

        //u8g2.setPowerSave(1);
        //lcdStandbyTime = millis();
        lastDebounceTime = millis();

        //Serial.println("Pressed!");
        if (enterMenu == false) //if not enter the menu, then change the data
        {

            if (weatherMenu)
            {
                weatherMenu = false;
            }
            else
            {
                weatherMenu = true;
            }
        }
        else
        {



            menuStandbyTime = millis();
            switch (currentMenu)
            { //different number reprensent different menu interface; 0:main menu 1:manual watering 2:record 3:record for each sensor 4:setting 5:settings for differnet sensors 6:adjust menu
            case 0: //main menu
                switch (currentItem)
                {
                case 0:
                    currentMenu = 1;
                    currentItemForLastPage = 0;
                    currentItem = 0;
                    break;
                case 1:
                    currentMenu = 2;
                    currentItemForLastPage = 1;
                    currentItem = 0;
                    break;
                case 2:
                    currentMenu = 4;
                    currentItemForLastPage = 2;
                    currentItem = 0;
                    break;
                case 3:
                    currentItem = 0;
                    enterMenu = false;
                    //Serial.print("exit!");
                    break;
                }
                break;

            case 1: //manual watering
                if (currentItem == sensors.size())
                { //back
                    currentItem = currentItemForLastPage;
                    currentMenu = 0;
                    //Serial.print("back!");
                }
                else
                {
                    sensors[currentItem].setWater();
                }
                break;

            case 2: //settings
                Serial.println("settings");
                if (currentItem == sensors.size())
                {

                    currentItem = currentItemForLastPage;
                    currentMenu = 0;
                }
                else
                {
                    currentMenu = 3; //enter settings menu for each sensor
                    Serial.println(currentItem);
                    currentItemForLastPage = currentItem;
                    currentItem = 0;
                    Serial.print(currentItemForLastPage);
                    Serial.println(currentItem);
                }
                break;

            case 3://setting menu for each sensor
                if (currentItem == 4) //back
                {
                    currentItem = currentItemForLastPage;
                    currentMenu = 2;
                }
                else
                {
                    currentMenu = 6; //enter the settings  for each element
                    sensorID = currentItemForLastPage;
                    currentItemForLastPage = currentItem;
                    switch (currentItem)
                    {
                    case 0:
                        currentItem = sensors[sensorID].getTempertureLowerLimit();
                        break;
                    case 1:
                        currentItem = sensors[sensorID].getTempertureUpperLimit();
                        break;
                    case 2:
                        currentItem = sensors[sensorID].getHumidityLowerLimit();
                        break;
                    case 3:
                        currentItem = sensors[sensorID].getHumidityUpperLimit();
                        break;
                    }
                }
                break;

            case 4: //records

                if (currentItem == sensors.size())
                {
                    currentItem = currentItemForLastPage;
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
               // if (currentItem == 0)
               // {
                    currentItem = currentItemForLastPage;
                    currentMenu = 4;
                    //sensors[currentItemForLastPage].setRecord(false);
                //}
                //else
                //{
                    //sensors[currentItemForLastPage].setRecord(true);
                //}
                break;

            case 6:
                currentMenu = 3;
                switch (currentItemForLastPage)
                {
                case 0:
                    sensors[sensorID].setTempertureLowerLimit(currentItem);

                    Serial.print("sensorID");
                    Serial.println(sensors[sensorID].getTempertureLowerLimit());
                    Serial.println(currentItem);
                    break;
                case 1:
                    sensors[sensorID].setTempertureUpperLimit(currentItem);
                    break;
                case 2:
                    sensors[sensorID].setHumidityLowerLimit(currentItem);
                    break;
                case 3:
                    sensors[sensorID].setHumidityUpperLimit(currentItem);
                    break;
                }
                currentItem = currentItemForLastPage;
                break;
            }
        }
        debounce = false;
    }
}

void readQuadrature()
{
    //u8g2.setPowerSave(1);
    //lcdStandbyTime = millis();
    menuStandbyTime = millis();
    if (int_nu == 0 && digitalRead(3 /*has been changed with B, if any bugs appear with encoder, check this*/) == 0x0)
    {

        flag = 0;
        if (digitalRead(2))
        {
            flag = 1;
        }
        int_nu = 1;
    }
    if (int_nu && digitalRead(3 /*has been changed with B, if any bugs appear with encoder, check this*/))
    {
        if (digitalRead(2) == 0x0 && flag == 1)
        {
            switch (currentMenu)
            { //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor 7:adjust menu
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
                if (currentItem <= 0)
                    currentItem = 0;
                else
                    currentItem++;
                break;

            case 6:
                if (currentItem >= 99)
                    currentItem = 99;
                else
                    currentItem++;
                break;
            }
        }

        if (digitalRead(2) && flag == 0)
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
        Serial.print("Last: ");
        Serial.println(currentItemForLastPage);
        Serial.print("Menu: ");
        Serial.println(currentMenu);
        Serial.println("--------");
    }
}

void connection()
{
    //Serial.print(connectionCountdown);

    if (connectionCountdown == 0)
    {
        ifConnected = false;
        signalStrength = "00%";
    }
    else
    {
        ifConnected = true;
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
    for (int i = 0; i < sensors.size(); i++)
    {
        int chk = DHT11.read(5 /*define the pin2 is the airTempHumidity sensor*/);
        if (chk == "DHTLIB_OK")
        {
        if ((sensors[i].getTempertureLowerLimit() < DHT11.temperature) && (sensors[i].getTempertureUpperLimit() > DHT11.temperature) && (sensors[i].getHumidityLowerLimit() > sensors[i].getCurrentHumidity()))
        {
            sensors[i].setWater();
            //Serial.println("Water");
        }
        }

    }
}

void readMoisture()
{
    for (int i = 0; i < sensors.size(); i++)
    {
        sensors[i].setCurrentHumidity();
    }
}

void readSettings()
{
    for (int i = 0; i < sensors.size(); i++)
    {
        sensors[i].readSavedSettings();
    }
}
