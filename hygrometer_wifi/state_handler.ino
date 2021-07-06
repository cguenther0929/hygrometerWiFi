#define HYG_EMAIL_ACC_KEY       "hyg_email_acc"
#define HYG_EMAIL_PASS_KEY      "hyg_email_pass"
#define RECIPIENT_EMAIL_KEY     "email_to"
#define WIFI_SSID_KEY           "wifi_ssid"
#define WIFI_PASS_KEY           "wifi_pass"
#define HUM_1_KEY               "hum1"
#define TEMP_1_KEY              "temp1"
#define HUM_2_KEY               "hum2"
#define TEMP_2_KEY              "temp2"
#define BAT_V_KEY               "bat_v"
#define BAT_SATUS_KEY           "bat_low"       //TODO is this variable now dead?  Remove?


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

            // current_state = WAITING_FOR_DATA;       //TODO take this line out

            if(digitalRead(SLEEP_BIT)) {     
                current_state = DEEP_SLEEP;
            }

            else {
                current_state = WAITING_FOR_DATA;
            }
        
        break;

        case WAITING_FOR_DATA:
            
            //TODO the following was in for testing only
            // WiFi.forceSleepBegin();         //TODO we may want to place this in other locations
            // delay(1);                   // There are claims a non-zero delay is required after calling force sleep

            // Serial.println(".");
            
            if (Serial.available() > 0){
                                
                SerialReadtoArray ();       // Blocking


                #if defined(ENABLE_LOGGING)
                    Serial.print("\Received string: "); Serial.println(data_input_string);
                #endif
            
            
                current_state = PARSE_INPUT_DATA;
            
            
            }  //Wait until serial data is available

            /**
             * ELSE
             * we contine to wait for new data...
             */

        break;
        
        case PARSE_INPUT_DATA:
        {
            int temp_length = 0;
            int b64len = 0;

            #if defined(ENABLE_LOGGING)
                Serial.println("\tParsing data.\n");
            #endif

            json_err = deserializeJson(json_doc,data_input_string);
            if(json_err) {

                #if defined(ENABLE_LOGGING)
                    Serial.println("Json deserialize error.");
                    Serial.print("Error: "); Serial.println(json_err.c_str());
                #endif
                
                return;
            }

            /**
             * Retrieve the hygrometer's email
             * address as passed in via JSON
             */
            const char * cc_hyg_email_addr = json_doc[HYG_EMAIL_ACC_KEY];
            memcpy(buf_hyg_email_address, cc_hyg_email_addr, strlen(cc_hyg_email_addr));
            
            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- Email address passed in via JSON: ");
                Serial.println(buf_hyg_email_address);
            #endif


            /**
             * Retrieve the hygrometer's email
             * login password as passed in via JSON
             */
            const char * cc_hyg_email_pass = json_doc[HYG_EMAIL_PASS_KEY];
            memcpy(buf_hyg_email_password, cc_hyg_email_pass, strlen(cc_hyg_email_pass));
            
            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- Email login password passed in via JSON: ");
                Serial.println(buf_hyg_email_password);
            #endif

            /**
             * Retrieve the recipient's
             * email address as passed in 
             * via JSON
             */
            const char * cc_recipient_email_addr = json_doc[RECIPIENT_EMAIL_KEY];
            memcpy(buf_recipient_email_addr, cc_recipient_email_addr, strlen(cc_recipient_email_addr));
            
            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- Recipient email address passed in via JSON: ");
                Serial.println(buf_recipient_email_addr);
            #endif
            
            /**
             * Retrieve WiFi SSID
             * as passed in via JSON
             */
            const char * cc_wifi_ssid = json_doc[WIFI_SSID_KEY];
            memcpy(buf_wifi_ssid, cc_wifi_ssid, strlen(cc_wifi_ssid));

            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- WiFi SSID passed in via JSON: ");
                Serial.println(buf_wifi_ssid);
            #endif

            /**
             * Retrieve WiFi password
             * as passed in via JSON
             */
            const char * cc_wifi_pass = json_doc[WIFI_PASS_KEY];
            memcpy(buf_wifi_password, cc_wifi_pass, strlen(cc_wifi_pass));

            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- WiFi password passed in via JSON: ");
                Serial.println(buf_wifi_password);
            #endif

            /**
             * Retrieve the first humidity
             * value as passed in via JSON
             */
            humidity_1 = json_doc[HUM_1_KEY];
            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- humidity 1 value passed in via JSON: ");
                Serial.println(humidity_1);
            #endif
            
            /**
             * Retrieve the second humidity
             * value as passed in via JSON
             */
            humidity_2 = json_doc[HUM_2_KEY];
            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- humidity 2 value passed in via JSON: ");
                Serial.println(humidity_2);
            #endif

            /**
             * Retrieve the first temperature 
             * value as passed in via JSON
             */
            temperature_1 = json_doc[TEMP_1_KEY];
            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- temperature 1 value passed in via JSON: ");
                Serial.println(temperature_1);
            #endif
            
            /**
             * Retrieve the first temperature 
             * value as passed in via JSON
             */
            temperature_2 = json_doc[TEMP_2_KEY];
            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- temperature 2 value passed in via JSON: ");
                Serial.println(temperature_2);
            #endif
            
            /**
             * Retrieve battery voltage  
             * value as passed in via JSON
             */
            battery_v = json_doc[BAT_V_KEY];
            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- battery voltage passed in via JSON: ");
                Serial.println(battery_v);
            #endif
            
            /**
             * Retrieve battery too low
             * flag as passed in via JSON
             */
            battery_too_low = json_doc[BAT_SATUS_KEY];
            #if defined(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- battery too low flag passed in via JSON: ");
                Serial.println(battery_too_low);
            #endif
            
            current_state = NETWORK_CONNECTION;          //TODO this is the line that should be in
        
        }
        break;
        

        case NETWORK_CONNECTION:
            
            WiFi.forceSleepWake();            /** TODO: might want to enable this or implement something similar */
            delay(1);                       // There are claims that a non-zero delay is required after calling the wake function

            if(!WiFiConnect(buf_wifi_ssid, buf_wifi_password)) {
                Serial.println("Deep sleep.");
                current_state = DEEP_SLEEP;
            }

            current_state = SEND_EMAIL;

        break;

        case SEND_EMAIL:
        {
            EMailSender emailSend(buf_hyg_email_address, buf_hyg_email_password);
            

            EMailSender::EMailMessage message;
            message.subject = "Hygrometer Status";
            
            AssembleEmailMessage();
            
            message.message = email_message;

            // EMailSender::Response resp = emailSend.send("clinton.guenther@gmail.com", message);      //Original line
            EMailSender::Response resp = emailSend.send(buf_recipient_email_addr, message);

            #if defined(ENABLE_LOGGING)
                Serial.println("");    
                Serial.println("Sending status: ");

                Serial.println(resp.status);
                Serial.println(resp.code);
                Serial.println(resp.desc);

                Serial.println("");
            #endif

            current_state = DEEP_SLEEP;         //TODO this is the line we want
            // current_state = WAITING_FOR_DATA;       //TODO this should be deep sleep


        }
        break;

        case DEEP_SLEEP:
            //TODO in for debugging only
            // digitalWrite(WIFI_ERR_LED, HIGH);
            digitalWrite(WIFI_ERR_LED, LOW);
            
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