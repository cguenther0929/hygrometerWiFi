/*
 * This is the main source file for the ESP8266 WiFi SOM
 * 
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>   //TODO does adding this in break anything?
#include <ArduinoJson.h>
#include <pins_arduino.h>
#include <Ticker.h>
#include "EMailSender.h"

/**
 * Uncomment the following 
 * if you wish to have logging 
 * data printed to the screen
 */
#define ENABLE_LOGGING      

/**
 * JSON and input
 * data constructors
 */
#define MAX_RX_BUF_INDEX          511
#define MAX_RX_BUF_ELEMENTS       MAX_RX_BUF_INDEX + 1
StaticJsonDocument<256> json_doc;
DeserializationError json_err;
char data_input_string[MAX_RX_BUF_ELEMENTS];

// FSM states
enum state {
  UNKNOWN,
  EVALUATE_SLEEP_BIT,
  WAITING_FOR_DATA,
  PARSE_INPUT_DATA,
  NETWORK_CONNECTION,
  SEND_EMAIL,
  DEEP_SLEEP
};

/**
 * State tracker 
 */
state current_state = UNKNOWN;

/**
 * The Sleep bit, driven
 * by the application processor,
 * defines when the WiFi module
 * shall go into deep sleep mode.  
 * Most likely, this bit will only
 * be looked at after booting.  
 */
#define SLEEP_BIT                 4

/**
 * WiFi Parameters
 */
#define WIFI_CONNECT_TIMEOUT_S    10

/**
 * Serial port 
 * parameters
 */
#define SER_TIMEOUT_MS            100


/**
 * Data imported 
 * from JSON string
 */
char buf_wifi_password[32];
char buf_wifi_ssid[32];

char buf_hyg_email_address[64];   // Email address hygrometer used to send emails
char buf_hyg_email_password[64];  // Account password for aforementioned email address 
char buf_recipient_email_addr[64];    // Email recipient

int humidity_1        = 0;
int humidity_2        = 0;
int temperature_1     = 0;
int temperature_2     = 0;
int battery_v         = 0;
bool battery_too_low  = false; 

/**
 * Email message to 
 * send
 */
String email_message = "";


/**
 * Set trip flags 
 * These are hard coded,
 * for now
 */
float           hyg_trip_point = 63;            // Minimum allowed humidity
float           bat_trip_voltage = 3.86;        // Minimum allowed battery voltage

/**
 * Timer parameters
 */
long            tmr1_write_val = 3030;   // Empirically derived to generate a 1ms tick timer.
unsigned int    ms_ticks_1;
unsigned int    ms_ticks_50;
unsigned int    ms_ticks_500;
unsigned int    ms_ticks_1000;

bool            Timer1msFlag = false;
bool            Timer50msFlag = false;
bool            Timer500msFlag = false;
bool            Timer1000msFlag = false;

long            seconds_counter = 0;        //32bit value 4.264....e9



void ICACHE_RAM_ATTR onTimerISR(){
  timer1_write(tmr1_write_val);

  Timer1msFlag = true;

  if(ms_ticks_1 == 50) {
    ms_ticks_1 = 0;
    Timer50msFlag = true;
    if(ms_ticks_50 == 10) {
      ms_ticks_50 = 0;               //Reset centi-seconds
      Timer500msFlag = true;
    
      if(ms_ticks_500 == 2) {         //See if we need to roll over
        ms_ticks_500 = 0;
        Timer1000msFlag = true;  
      }
      else {
        ms_ticks_500++;              //Increase 500ms timer
      }

    }
    else {
        ms_ticks_50++;
    }
    
  }
  else {
    ms_ticks_1++;
  }
  

}


void loop(void)
{

  if(Timer1msFlag == true) {
    Timer1msFlag = false;
    StateEvaluation();
  }
  
  if(Timer50msFlag == true) {
    Timer50msFlag = false;
  }

  if(Timer500msFlag == true) {
    Timer500msFlag = false;
  }

  if(Timer1000msFlag == true) {
    Timer1000msFlag = false;
    // digitalWrite(LED_1,!(digitalRead(LED_1)));  //Toggle LED Pin
    (seconds_counter == 300000)?(seconds_counter = 0):(seconds_counter++);
    
  }
}



void setup(void) {
  
  /**
   * Setup pin directions
   */
  pinMode(SLEEP_BIT, INPUT);

  Serial.begin(115200);
  Serial.setTimeout(50);    //Timeout value in ms -- max is 1000
  
  current_state = EVALUATE_SLEEP_BIT;

  //Initialize Ticker every 0.05s
  timer1_attachInterrupt(onTimerISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(tmr1_write_val);        //.05s 

  #if defined(ENABLE_LOGGING)
    delay(2000);
    Serial.println("Module just rebooted.");
  #endif

}