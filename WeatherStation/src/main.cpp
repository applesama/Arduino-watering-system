
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Wire.h>
#include <string.h>

#define LED D4
#define DEBUG               //if turn on debug
#define AP_SSID ""       //wifi name
#define AP_PSW "" //wifi password

#ifdef DEBUG
#define DebugPrintln(message) Serial.println(message)
#else
#define DebugPrintln(message)
#endif

#ifdef DEBUG
#define DebugPrint(message) Serial.print(message)
#else
#define DebugPrint(message)
#endif

bool autoConfig();
void smartConfig();
bool sendRequest(const char *host, const char *cityid, const char *apiKey);
bool skipResponseHeaders();
void readReponseContent(char *content, size_t maxSize);
void stopConnect();
void clrEsp8266ResponseBuffer(void);
bool parseUserData(char *content, struct UserData *userData);
void requestEvent();
void receiveEvent(int howMany);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 28800); //set time request


const unsigned long HTTP_TIMEOUT = 5000; // max respone time from server
const size_t MAX_CONTENT_SIZE = 500;     // max size of the HTTP response
const char *host = "";
const char *APIKEY = ""; //API KEY
const char *city = "";
const char *language = "en"; //English

int flag = HIGH; //set the light is flase;

bool ifUpdated = false;
String dataReceived = "";//for i2c

WiFiClient client;
char response[MAX_CONTENT_SIZE];
char endOfHeaders[] = "\r\n\r\n";

long lastTime = 0;
// requesting time gap

// structer used to
struct UserData
{
    char city[16];        //city name
    char weather_code[4]; //code of weather
    char temp[5];         //temperature
};

UserData userData;


void setup()
{
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    WiFi.disconnect();
    if (!autoConfig())
    {
        smartConfig();
        DebugPrint("Connecting to WiFi"); //Some tips
        while (WiFi.status() != WL_CONNECTED)
        {
            //check if wifi is connected
            delay(500);
            DebugPrint(".");
        }
    }

    delay(1000);
    digitalWrite(LED, LOW);
    DebugPrintln("IP address: ");
    DebugPrintln(WiFi.localIP()); //WiFi.localIP()return the ip that we got
    lastTime = millis();
    timeClient.begin();
    Wire.begin(8);                // join i2c bus with address 8
    Wire.onReceive(receiveEvent); // register receive event
    Wire.onRequest(requestEvent); // register request event
    //watchDog timegap
    ESP.wdtEnable(5000);
}

void loop()
{
  
    while (!client.connected())
    {
        if (!client.connect(host, 80))
        {
            flag = !flag;
            digitalWrite(LED, flag);
            delay(500);
            //feed dog
            ESP.wdtFeed();
        }
    }
    timeClient.update();
    if (millis() - lastTime >= 20000)
    {
        //update the data every 20 seconds
        lastTime = millis();
        if (sendRequest(host, city, APIKEY) && skipResponseHeaders())
        {
            clrEsp8266ResponseBuffer();
            readReponseContent(response, sizeof(response));
            
            if (parseUserData(response, &userData))
            {
                ifUpdated = true;
                Serial.print(userData.city);
                Serial.print(userData.weather_code);
                Serial.print(userData.temp);
                Serial.print(timeClient.getDay());
                Serial.println(timeClient.getFormattedTime());
            }
        }
    }
    ESP.wdtFeed();
    pinMode(LED, OUTPUT);
    delay(400);
    digitalWrite(LED, HIGH);
    
}


bool autoConfig()
{
    WiFi.mode(WIFI_AP_STA); //set it as station mode
    WiFi.begin(AP_SSID, AP_PSW);
    delay(2000); //just delay 2s for stablelizating
    DebugPrintln("AutoConfiging ......");
    for (int index = 0; index < 10; index++)
    {
        int wstatus = WiFi.status();
        if (wstatus == WL_CONNECTED)
        {
            DebugPrintln("AutoConfig Success");
            DebugPrint("SSID:");
            DebugPrintln(WiFi.SSID().c_str());
            DebugPrint("PSW:");
            DebugPrintln(WiFi.psk().c_str());
            return true;
        }
        else
        {
            DebugPrint(".");
            delay(500);
            flag = !flag;
            digitalWrite(LED, flag);
        }
    }
    DebugPrintln("AutoConfig Faild!");
    return false;
}


void smartConfig()
{
    WiFi.mode(WIFI_STA);
    delay(1000);
    DebugPrintln("Wait for Smartconfig");
    // wait for config
    WiFi.beginSmartConfig();
    while (1)
    {
        DebugPrint(".");
        delay(200);
        flag = !flag;
        digitalWrite(LED, flag);

        if (WiFi.smartConfigDone())
        {
            //smartconfig finished
            DebugPrintln("SmartConfig Success");
            DebugPrint("SSID:");
            DebugPrintln(WiFi.SSID().c_str());
            DebugPrint("PSW:");
            DebugPrintln(WiFi.psk().c_str());
            WiFi.mode(WIFI_AP_STA); //station mode
            WiFi.setAutoConnect(true);
            break;
        }
    }
}

bool sendRequest(const char *host, const char *cityid, const char *apiKey)
{
    // We now create a URI for the request
    String GetUrl = "/v3/weather/now.json?key=";
    GetUrl += apiKey;
    GetUrl += "&location=";
    GetUrl += city;
    GetUrl += "&language=";
    GetUrl += language;
    // This will send the request to the server
    client.print(String("GET ") + GetUrl + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    DebugPrintln("create a request:");
    DebugPrintln(String("GET ") + GetUrl + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n");
    delay(1000);
    return true;
}

bool skipResponseHeaders()
{
    // HTTP headers end with an empty line
    bool ok = client.find(endOfHeaders);
    if (!ok)
    {
        DebugPrintln("No response or invalid response!");
    }
    return ok;
}

void readReponseContent(char *content, size_t maxSize)
{
    size_t length = client.readBytes(content, maxSize);
    delay(100);
    DebugPrintln("Get the data from Internet!");
    content[length] = 0;
    DebugPrintln(content);
    DebugPrintln("Read data Over!");
    client.flush();
}

// stop the http connection
void stopConnect()
{
    client.stop();
}

void clrEsp8266ResponseBuffer(void)
{
    memset(response, 0, MAX_CONTENT_SIZE); //clear it
}

bool parseUserData(char *content, struct UserData *userData)
{

    DynamicJsonBuffer jsonBuffer;

    JsonObject &root = jsonBuffer.parseObject(content);

    if (!root.success())
    {
        Serial.println("JSON parsing failed!");
        return false;
    }
    
    strcpy(userData->city, root["results"][0]["location"]["name"]);
    strcpy(userData->weather_code, root["results"][0]["now"]["code"]);
    strcpy(userData->temp, root["results"][0]["now"]["temperature"]);
    
    return true;
}
//timeClient.getFormattedTime();
//Serial.println(timeClient.getDay()
void receiveEvent(int howMany)
{
    Serial.println("Receive");
    while (0 < Wire.available())
    {
        dataReceived = dataReceived + Wire.read(); /* receive byte as a character */
    }
    DebugPrintln(dataReceived);
    Serial.println(dataReceived);
}
// function that executes whenever data is requested from master
void requestEvent()
{
    Serial.println("Request");
    /*if(dataReceived == "update"){
        Wire.write(userData.city);//city name has not been formatted, be aware of this, city name may be longer than current length
        //4
        //if(strlen(userData.weather_code) == 1) Wire.write("0");
        Wire.write(userData.weather_code);
        //2
        //if(strlen(userData.temp) == 1) Wire.write("0");
        Wire.write(userData.temp);
        //2
        Wire.write(timeClient.getDay());
        //1
        //int temp = timeClient.getHours();
        //if(temp < 10) Wire.write("0");
        Wire.write(timeClient.getHours());
        //2
        temp = timeClient.getMinutes();
        //if(temp < 10) Wire.write("0");
        Wire.write(temp);
        //2
        temp = timeClient.getSeconds();
        //if(temp < 10) Wire.write("0");
        Wire.write(temp);
    }
    else if((dataReceived == "signal")&&(client.connected())){
        Wire.write(WiFi.RSSI());
    }*/
    //dataReceived = "";
}