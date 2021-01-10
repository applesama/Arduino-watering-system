# 1 "f:\\WaterArduino\\watering\\watering.ino"

# 3 "f:\\WaterArduino\\watering\\watering.ino" 2
# 4 "f:\\WaterArduino\\watering\\watering.ino" 2
# 5 "f:\\WaterArduino\\watering\\watering.ino" 2

# 7 "f:\\WaterArduino\\watering\\watering.ino" 2


using namespace std;




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
    const int mHumidityPort[4] = {14, 15, 16, 17}; //All ports for humidity sensors
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
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2((&u8g2_cb_r0), /* clock=*/SCL, /* data=*/SDA, /* reset=*/255); // All Boards without Reset of the Display
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
        pinMode(mSensorPort, 0x0);
    }

    void setServoPort(int port)
    {
        mServoPort = port;
        pinMode(mServoPort, 0x1);
    }

    void watering()
    { // loop will keep call this function all the time
        if (mWaterFlag)
        {
            digitalWrite(mServoPort, 0x0);
            delay(5000);
            digitalWrite(mServoPort, 0x1);
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
# 185 "f:\\WaterArduino\\watering\\watering.ino"
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
{ //first is sensor port and second one is servo
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
const char *mainMenuItem[3] = {"Manual watering", "Auto-Watering settings", "Sensor records"};
const char *menuItemFor6[4] = {"UpperTemputerture", "LowerTemputerture", "UpperHumidity", "LowerHumidity"};

uint8_t currentMenu = 1; //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor
uint8_t currentPage = 1;
uint8_t currentItem = 0;
uint8_t currentItemForLastPage = 1; //record which item selected in last item;

uint8_t sensorThatCurrentHave = 0; //how many sensor we have

uint8_t int_nu = 0; //for rotary encoder
uint8_t flag = 0;

//part of the variables are still public for debug, set
void setup()
{
    Serial.begin(9600);
    pinMode(5 /*define the pin2 is the airTempHumidity sensor*/, 0x1); //to send data to DHT11 for hand shaking, so using output
    pinMode(3 /*has been changed with B, if any bugs appear with encoder, check this*/, 0x0); //for encoder A pin and B pin
    pinMode(2, 0x0);
    pinMode(5, 0x0); //for te switch on encoder
    //attachInterrupt(1, readQuadrature, CHANGE);
    attachInterrupt(0, readQuadrature, 1);
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

            drawMenu();
        }

    } while (u8g2.nextPage());

    delay(1000);
}

void drawMenu()
{
    vector<char*> items; //used to store how many items in a menu
    uint8_t i, h;
    u8g2_uint_t w, d;

    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.setFontRefHeightText();
    u8g2.setFontPosTop(); //以左上角为坐标原点
    u8g2.drawLine(u8g2.getDisplayWidth(), 15, 0, 15); //a horizontal line
    h = u8g2.getAscent() - u8g2.getDescent(); //height of fonts
    w = u8g2.getDisplayWidth();

    u8g2.drawStr((u8g2.getDisplayWidth() / 2) - ((u8g2.getStrWidth("Main Menu") / 2)), 1, "Main Menu");


    switch (currentMenu)
    {
    case 1:
        for (int n = 0; n < 3; n++)
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
        u8g2.setFont(u8g2_font_6x13_tf);
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
    u8g2_uint_t d;

    u8g2.setFontRefHeightText(); // Ascent will be the ascent of "A" or "1" of the current font. Descent will be the descent "g" of the current font (this is the default after startup).
    u8g2.setFontPosTop(); //set the lefttop as  (0,0)
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.drawGlyph(d, 2, 0x00f8); //signal sign
    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.drawStr(d + 50, 2, "Time");
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
        { //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor 7:adjust menu
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
            case 1: //main menu
                if (currentItem == 3)
                    currentItem = 3;
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
                if (currentItem == 1)
                    currentItem = 1;
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
                if (currentItem == 4)
                    currentItem = 4;
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
    }
    Serial.print("Item: ");
    Serial.println(currentItem);
    Serial.print("Menu: ");
    Serial.println(currentMenu);
    Serial.println("--------");
}
void restMenuData()
{
    currentMenu = 1; //different number reprensent different menu interface; 1:main menu 2:manual watering 3:record 4:record for each sensor 5:setting 6:setting for differnet sensor
    currentPage = 1;
    currentItem = 0;
    currentItemForLastPage = 1;
}
