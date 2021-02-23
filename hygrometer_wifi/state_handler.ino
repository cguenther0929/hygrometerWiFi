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


// Gsender *gsender = Gsender::Instance();    // Get handle to Gsender
// const char * subject = "Humidity Critical";

// const char * email_body = "This is just a test";

/* Setup initial state conditions */
void StateInitialize( void ) {
    delay(1);
  
}

/**
 * TOOD:
 * how about placing the ESP module 
 * in a low power sleep mode (not deep sleep)
 * until it's time to connect and send an 
 * email.  
 */

void StateEvaluation( void ) {      //TODO: see babybot for example 

    switch (current_state) {
        case EVALUATE_SLEEP_BIT:
            /**
             * If sleep bit is set, 
             * transition to DEEP_SLEEP
             * state
             */

            if(digitalRead(SLEEP_BIT)) {
                current_state = DEEP_SLEEP;
            }

            else {
                current_state = WAITING_FOR_DATA;
            }
        
        break;

        case WAITING_FOR_DATA:
            
            WiFi.forceSleepBegin();
            delay(1);                   // There are claims a non-zero delay is required after calling force sleep
            
            if (!Serial.available()){
                SerialReadtoArray ();       // Blocking
            }  //Wait until serial data is available


            /**
             * TODO: remove stale code 
             */
            // Serial.print("Received string: "); Serial.println(data_input_string);

            current_state = PARSE_INPUT_DATA;

        break;
        
        // case PARSE_INPUT_DATA:
        //     int temp_length = 0;

        //     json_err = deserializeJson(json_doc,data_input_string);
        //     if(json_err) {
        //         Serial.print("Error: "); Serial.println(json_err.c_str());
        //         return;
        //     }

        //     const char * cc_hyg_email_addr = json_doc["hyg_email_acc"];
        //     memcpy(buf_hyg_email_address, cc_hyg_email_addr, strlen(cc_hyg_email_addr));

        //     const char * cc_hyg_email_pass = json_doc["hyg_email_pass"];
        //     memcpy(buf_hyg_email_password, cc_hyg_email_pass, strlen(cc_hyg_email_pass));

        //     const char * cc_wifi_ssid = json_doc["wifi_ssid"];
        //     memcpy(buf_wifi_ssid, cc_wifi_ssid, strlen(cc_wifi_ssid));

        //     const char * cc_wifi_pass = json_doc["wifi_pass"];
        //     memcpy(buf_wifi_password, cc_wifi_pass, strlen(cc_wifi_pass));

        //     const char * cc_humidity_1 = json_doc["hum1"];
        //     memcpy(buf_humidity_1, cc_humidity_1, strlen(cc_humidity_1));
            
        //     const char * cc_humidity_2 = json_doc["hum2"];
        //     memcpy(buf_humidity_2, cc_humidity_2, strlen(cc_humidity_2));

        //     const char * cc_temperature_1 = json_doc["temp1"];
        //     memcpy(buf_temperature_1, cc_temperature_1, strlen(cc_temperature_1));

        //     const char * cc_temperature_2 = json_doc["temp2"];
        //     memcpy(buf_temperature_2, cc_temperature_2, strlen(cc_temperature_2));
            
        //     const char * cc_battery_voltage = json_doc["bat_v"];
        //     memcpy(buf_battery_voltage, cc_battery_voltage, strlen(cc_battery_voltage));

        //     battery_too_low = json_doc["bat_low"];
            
        //     /**
        //      * Base 64 Encode the email address
        //      * as well as the password to the 
        //      * email account.  
        //      */
        //     temp_length = strlen(cc_hyg_email_addr);
        //     Base64.encode(buf_hyg_email_address_b64, buf_hyg_email_address, temp_length);

        //     temp_length = strlen(cc_hyg_email_pass);
        //     Base64.encode(buf_hyg_email_password_b64, buf_hyg_email_password, temp_length);

        //     current_state = NETWORK_CONNECTION;
        
        // break;
        

        case NETWORK_CONNECTION:
            // WiFi.forceSleepEnd();            /** TODO: might want to enable this or implement something similar */
            
            delay(1);               // There are claims that a non-zero delay is required after calling END

            if(!WiFiConnect(buf_wifi_ssid, buf_wifi_password)) {
                current_state = DEEP_SLEEP;
            }

            current_state = SEND_EMAIL;

            /**
             * TODO: Add code for making
             * network connection...
             */
        break;

        case SEND_EMAIL:
        {

            Gsender *gsender = Gsender::Instance();    // Get handle to Gsender

            // memcpy()
            // gsender -> EMAILBASE64_PASSWORD = "just a test";

            const char * subject = "Humidity Critical";

            const char * email_body = "This is just a test";
            // email_body =  "Humidity Critical: " + (String)global_hyg_val + 
                    // "% (critical trip point set to: " + (String)hyg_trip_point + "%).";
            
            
            gsender -> SetPassword("Justatest");
            
            if(
                (gsender->Subject(subject)->Send(buf_to_email_address, email_body))
                ) 
            {
                __asm__("nop\n\t");
            } 
            
            else {
                Serial.print("Error sending message: ");
                Serial.println(gsender->getError());
            }

            current_state = DEEP_SLEEP;


        }
        break;

        case DEEP_SLEEP:
            /**
             * Put module into deep sleep.
             * Only a reset can wake the module!
             */
            ESP.deepSleep(0);
        break;

        default:
            delay(1);
    }
}