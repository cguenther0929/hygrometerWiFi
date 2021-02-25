/*
 * This is the main source file for the ESP8266 WiFi SOM
 * 
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
// #include <Base64.h>
// #include <ESP8266WebServer.h>
// #include <Ticker.h>
// #include "SPI.h"   //TODO remove
// #include <SPISlave.h.h>  // TODO: remove?  
#include <pins_arduino.h>
#include "Gsender.h"
#include "base64.h"


/**
 * JSON and input
 * data constructors
 */
#define MAX_RX_BUF_INDEX          255
#define MAX_RX_BUF_ELEMENTS       MAX_RX_BUF_INDEX + 1
StaticJsonDocument<256> json_doc;
DeserializationError json_err;
char data_input_string[MAX_RX_BUF_ELEMENTS];

// #define HYG_EMAIL_ACC_KEY       "hyg_email_acc"
// #define HYG_EMAIL_PASS_KEY      "hyg_email_pass"
// #define WIFI_SSID_KEY           "wifi_ssid"
// #define WIFI_PASS_KEY           "wifi_pass"
// #define HUM_1_KEY               "hum1"
// #define TEMP_1_KEY              "temp1"
// #define HUM_2_KEY               "hum2"
// #define TEMP_2_KEY              "temp2"
// #define BAT_V_KEY               "bat_v"
// #define BAT_SATUS_KEY           "bat_low"


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
#define WIFI_CONNECT_TIMEOUT_S    5

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
char buf_hyg_email_address[32];   // Email address hygrometer used to send emails
char buf_hyg_email_password[32];  // Account password for aforementioned email address 
char buf_to_email_address[32];    // Email recipient

char buf_humidity_1[8];
char buf_humidity_2[8];
char buf_temperature_1[8];
char buf_temperature_2[8];
char buf_battery_voltage[8];

char buf_hyg_email_address_b64[64];   // Hygrometer's email address, BASE64 encoded
char buf_hyg_email_password_b64[64];  // Password, BASE64, of aforementioned email account

bool battery_too_low = false; 

/**
 * Buffers for storing data 
 * related to WiFi
 */
// char ssid_buffer[32];      //TODO TODO Put these back in!!!
// char wifi_pass[32];


/**
 * TODO take the following out after 
 * the aforementioned buffers are enabled
 * TODO: cleanup stale code
 */
// const char*     ssid = "CJG_GbE_2G4";           // My Home Router SSID  //TODO cleanup lines?
// const char*     password = "GlockHK23";         // My Home Router Password

/**
 * Set trip flags 
 * These are hard coded,
 * for now
 */
float           hyg_trip_point = 63;            // Minimum allowed humidity
float           bat_trip_voltage = 3.86;        // Minimum allowed battery voltage

// String          html_string = "";               // Will be populated as HTML and served up

/** TODO: cleanup stale code  */
// int       PWR_GOOD = 4;     //GPIO #4 'looks' at the power input status.  If logic zero, we've lost AC input power

/**
 * Timer parameters
 */
long            tmr1_write_val = 151515;   // Empirically derived to generate a 50ms tick timer.
unsigned int    ms_ticks_50;
unsigned int    ms_ticks_500;
unsigned int    ms_ticks_1000;

bool            Timer50msFlag = false;
bool            Timer500msFlag = false;
bool            Timer1000msFlag = false;
bool            timer_pause_1 = false;      //This will prevent a loop from running multiple times during its current second
bool            timer_pause_2 = false;     

long            seconds_counter = 0;        //32bit value 4.264....e9

// TEMPERATURE AND POWER
float           temp_buffer[8];                       // Keep FIFO buffer for temperature averaging
// int             circle_buf_ptr = 0;                   // Keep track of which sample to 
float           global_hyg_val = -0.0;               // Value updated during analog read of temperature
bool            debug_print = true;                   // Set this to true when we want to print debug messages

bool            critical_condition_detected = false;      // Send immediate warning if freezer temp is critical, but then send periodic after that
bool            initial_warning_sent = false;             //This flag is set only inside the loop whose job is to send warning emails
bool            humidity_is_critical = false;                 // This flag is set when the measured temperature is warmer than the trip point
// bool            pwr_is_good = true;                       // This flag is cleared if we loose AC input power
bool            reboot_POST = false;                      // We want to send an email at restart.  





void setup(void) {
  
  /**
   * Setup pin directions
   */
  pinMode(SLEEP_BIT, INPUT);

  /** 
   * TODO: The following is test code
   * and needs to be removed
  */
  // char * my_test_string = "whatsup";
  // int temp_length = strlen(my_test_string);
  // int b64len = b64_encode(buf_hyg_email_address_b64, my_test_string, strlen(my_test_string));
  // delay(100);
  
  
  Serial.begin(115200);
  Serial.setTimeout(50);    //Timeout value in ms -- max is 1000
  
  
  Serial.println("");

  //Initialize Ticker every 0.05s
  // timer1_attachInterrupt(onTimerISR);
  // timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  // timer1_write(tmr1_write_val);        //.05s 

  // if(debug_print) {
  //   Serial.println("****DEBUG Rebooted");
  // }

}

void loop(void)
{
  // String    subject       = "";
  // String    email_body    = "";
  

  // server.handleClient();

  if(Timer50msFlag == true) {
    Timer50msFlag = false;
    StateEvaluation();
  }

  delay(50);

  // if(Timer500msFlag == true) {
  //   Timer500msFlag = false;
  //   // global_hyg_val = get_temperature();
  // }

  // if(Timer1000msFlag == true) {
  //   Timer1000msFlag = false;
  //   // digitalWrite(LED_1,!(digitalRead(LED_1)));  //Toggle LED Pin
  //   (seconds_counter == 300000)?(seconds_counter = 0):(seconds_counter++);
  //   timer_pause_1 = false;
  //   timer_pause_2 = false;
    
  // }

  // if(seconds_counter % 5 == 0 & timer_pause_2 == false) {                  // Check temperature and power status
  //   timer_pause_2 = true;
    
    // global_hyg_val = get_temperature();
    
    //Check to see if the temperature value is above the set threshold.  Do not care if freezer temp is negative since trip point will always be zero or positive
    // ((global_hyg_val > hyg_trip_point) & !temp_is_neg) ? (humidity_is_critical = true):(humidity_is_critical = false);
    // ((global_hyg_val > hyg_trip_point)) ? (humidity_is_critical = true):(humidity_is_critical = false);
    
    // if(debug_print) {
    //   Serial.print("****DEBUG Temp:"); Serial.println(global_hyg_val);
    //   if(humidity_is_critical)
    //     Serial.println("****DEBUG Temp critical");
    // }
      

    // //Check power (AC Mains) status  //TODO can remove this
    // pwr_is_good = digitalRead(PWR_GOOD);   

    // if((humidity_is_critical) {  
    //   critical_condition_detected = true;
    //   digitalWrite(LED_2,1);              //Illuminate red LED to indicate a problem.  
    // }

    // if(!humidity_is_critical) {
    //   critical_condition_detected = false;
    //   initial_warning_sent = false;
    //   digitalWrite(LED_2,0);              //Illuminate red LED to indicate a problem.  
    // }         

  // }
  
  // if(
  //   ((seconds_counter % 86400 == 0) & timer_pause_1 == false)                | 
  //   (critical_condition_detected == true & initial_warning_sent == false)     |
  //   (humidity_is_critical & (seconds_counter % 600 == 0) & timer_pause_1 == false) |
  //   (reboot_POST == false)
  //   ) {

  //   timer_pause_1 = true;  
  //   initial_warning_sent = true;              //Initial warning sent, so set flag to prevent subsequent messages

  //   if(debug_print)
  //   {
  //     Serial.println("****DEBUG Email sending loop");  
  //     Serial.println("****DEBUG Seconds Counter: " + String(seconds_counter));  
  //   }
      

  //   Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance

  //   // Humidity is critical so send email 
  //   if(humidity_is_critical) {
  //     subject = "Humidity Critical";
  //     email_body =  "Humidity Critical: " + (String)global_hyg_val + 
  //                   "% (critical trip point set to: " + (String)hyg_trip_point + "%).";
  //     if(
  //       (gsender->Subject(subject)->Send("clinton.guenther@gmail.com", email_body))
  //       ) 
  //     {
  //         __asm__("nop\n\t");
  //     } 
      
  //     else {
  //       Serial.print("Error sending message: ");
  //       Serial.println(gsender->getError());
  //     }
  //   }

    // AC mains power issue, so send email
    // else if(!pwr_is_good) {
    //   subject = "AC Mains Power Down";
    //   email_body =  "AC mains power is down.\r\nCurrent Freezer Temp: " + (String)global_hyg_val + 
    //                 "&#176F (critical trip point set to: " + (String)hyg_trip_point + "&#176F).";
      
    //   if(
    //     (gsender->Subject(subject)->Send("meghanspindler12@hotmail.com", email_body)) &
    //     (gsender->Subject(subject)->Send("clinton.guenther@gmail.com", email_body))
    //     ) 
    //   {
    //     __asm__("nop\n\t");
    //   } 
      
    //   else {
    //     Serial.print("Error sending message: ");
    //     Serial.println(gsender->getError());
    //   }
    // }
    
    // Everything normal, so send heartbeat email  
    // else {
    //   subject = "Hygrometer Update";
    //   email_body =  "Hygrometer Healthy. Current humidity: " + (String)global_hyg_val + 
    //                 "% (critical trip point set to: " + (String)hyg_trip_point + "%).";
    //   if(
    //     (gsender->Subject(subject)->Send("clinton.guenther@gmail.com", email_body))
    //     ) 
    //   {
    //     __asm__("nop\n\t");
    //   } 
      
    //   else {
    //     Serial.print("Error sending message: ");
    //     Serial.println(gsender->getError());
    //   }
    // }
    
    // Freezer alarm just rebooted
    // if(reboot_POST == false) {
    //   subject = "Hygrometer Just Booted";
    //   email_body = "Hygrometer just booted.";
      
    //   if(
    //     (gsender->Subject(subject)->Send("clinton.guenther@gmail.com", email_body))
    //     ) 
    //   {
    //     __asm__("nop\n\t");
    //   } 
      
    //   else {
    //     Serial.print("Error sending message: ");
    //     Serial.println(gsender->getError());
    //   }
    //   reboot_POST = true;     // Set flag that indicates we've sent the reboot post.  

    // }

  // }
}

// void ICACHE_RAM_ATTR onTimerISR(){
//   timer1_write(tmr1_write_val);

//   Timer50msFlag = true;

//   if(ms_ticks_50 == 10) {
//     ms_ticks_50 = 0;               //Reset centi-seconds
//     Timer500msFlag = true;
  
//     if(ms_ticks_500 == 2) {         //See if we need to roll over
//       ms_ticks_500 = 0;
//       Timer1000msFlag = true;  
//     }

//     else {
//       ms_ticks_500++;              //Increase 500ms timer
//     }

//   }
  
//   else {
//       ms_ticks_50++;
//   }

// }