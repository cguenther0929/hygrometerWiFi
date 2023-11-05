#define HYG_NAME_KEY            "hyg_name"
#define HYG_SMTP2GO_ACC_KEY     "hyg_smtp2go_acc"
#define HYG_SMTP2GO_PASS_KEY    "hyg_smtp2go_pass"
#define RECIPIENT_EMAIL_KEY     "email_to"
#define SENDER_EMAIL_KEY        "email_from"
#define WIFI_SSID_KEY           "wifi_ssid"
#define WIFI_PASS_KEY           "wifi_pass"
#define HUM_1_KEY               "hum1"
#define TEMP_1_KEY              "temp1"
#define HUM_2_KEY               "hum2"
#define TEMP_2_KEY              "temp2"
#define BAT_V_KEY               "bat_v"
#define BAT_SATUS_KEY           "bat_low"

void StateEvaluation( void ) {      

    switch (current_state) {
        
        /**
         * @brief STATE EVALUATE SLEEP BIT
         * 
         */
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

        /**
         * @brief WAITING FOR DATA STATE
         * 
         */
        case WAITING_FOR_DATA:
            
            if (Serial.available() > 0){
                                
                SerialReadtoArray ();       // Blocking

                if(ENABLE_LOGGING) {
                    Serial.print("\tReceived string: "); Serial.println(data_input_string);
                }
            
                current_state = PARSE_INPUT_DATA;
            }  //Wait until serial data is available

            /**
             * ELSE
             * we continue to wait for new data...
             */

        break;
        
        /**
         * @brief PARSE INPUT DATA STATE
         * 
         */
        case PARSE_INPUT_DATA:
        {
            int temp_length = 0;
            int b64len = 0;

            if(ENABLE_LOGGING)
                Serial.println("\tParsing data.\n");
            

            json_err = deserializeJson(json_doc,data_input_string);
            
            if(json_err) {

                if(ENABLE_LOGGING){
                    Serial.println("Json deserialize error.");
                    Serial.print("Error: "); Serial.println(json_err.c_str());
                }
                
                FlushSerialRXBuffer();
                current_state = WAITING_FOR_DATA;
                break;
            }

            /**
             * Retrieve the hygrometer's 
             * name 
             */
            const char * cc_hyg_name = json_doc[HYG_NAME_KEY];
            memcpy(buf_hyg_name, cc_hyg_name, strlen(cc_hyg_name));
            
            if(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- Email address passed in via JSON: ");
                Serial.println(buf_hyg_name);
            
            
            /**
             * Retrieve the hygrometer's email
             * address as passed in via JSON
             */
            const char * cc_hyg_smtp2go_acc = json_doc[HYG_SMTP2GO_ACC_KEY];
            memcpy(buf_hyg_smtp2go_account, cc_hyg_smtp2go_acc, strlen(cc_hyg_smtp2go_acc));
            
            // if(ENABLE_LOGGING)
            //     Serial.print("\tSTATE HDLR -- Email address passed in via JSON: ");
            //     Serial.println(buf_hyg_smtp2go_account);
            // 
            if(ENABLE_LOGGING){
                Serial.print("\tSTATE HDLR -- Email address passed in via JSON: ");
                Serial.println(buf_hyg_smtp2go_account);
            }


            /**
             * Retrieve the hygrometer's SMTP2GO
             * accounts password as passed in 
             * via JSON
             */
            const char * cc_hyg_smtp2go_pass = json_doc[HYG_SMTP2GO_PASS_KEY];
            memcpy(buf_hyg_smtp2go_password, cc_hyg_smtp2go_pass, strlen(cc_hyg_smtp2go_pass));
            
            if(ENABLE_LOGGING){
                Serial.print("\tSTATE HDLR -- Email login password passed in via JSON: ");
                Serial.println(buf_hyg_smtp2go_password);
            }

            /**
             * Retrieve the recipient's
             * email address as passed in 
             * via JSON
             */
            const char * cc_recipient_email_addr = json_doc[RECIPIENT_EMAIL_KEY];
            memcpy(buf_recipient_email_addr, cc_recipient_email_addr, strlen(cc_recipient_email_addr));
            
            // if(ENABLE_LOGGING)
            //     Serial.print("\tSTATE HDLR -- Recipient email address passed in via JSON: ");
            //     Serial.println(buf_recipient_email_addr);
            // 
            if(ENABLE_LOGGING){
                Serial.print("\tSTATE HDLR -- Recipient email address passed in via JSON: ");
                Serial.println(buf_recipient_email_addr);
            }
            
            /**
             * Retrieve sender's email
             * address 
             */
            const char * cc_sender_email_addr = json_doc[SENDER_EMAIL_KEY];
            memcpy(buf_sender_email_addr, cc_sender_email_addr, strlen(cc_sender_email_addr));
            
            if(ENABLE_LOGGING){
                Serial.print("\tSTATE HDLR -- Sender email address passed in via JSON: ");
                Serial.println(buf_sender_email_addr);
            }

            /**
             * Retrieve WiFi SSID
             * as passed in via JSON
             */
            const char * cc_wifi_ssid = json_doc[WIFI_SSID_KEY];
            memcpy(buf_wifi_ssid, cc_wifi_ssid, strlen(cc_wifi_ssid));

            // if(ENABLE_LOGGING)
            //     Serial.print("\tSTATE HDLR -- WiFi SSID passed in via JSON: ");
            //     Serial.println(buf_wifi_ssid);
            // 
            if(ENABLE_LOGGING) {
                Serial.print("\tSTATE HDLR -- WiFi SSID passed in via JSON: ");
                Serial.println(buf_wifi_ssid);
            }

            /**
             * Retrieve WiFi password
             * as passed in via JSON
             */
            const char * cc_wifi_pass = json_doc[WIFI_PASS_KEY];
            memcpy(buf_wifi_password, cc_wifi_pass, strlen(cc_wifi_pass));

            // if(ENABLE_LOGGING)
            //     Serial.print("\tSTATE HDLR -- WiFi password passed in via JSON: ");
            //     Serial.println(buf_wifi_password);
            // 
            if(ENABLE_LOGGING) {
                Serial.print("\tSTATE HDLR -- WiFi password passed in via JSON: ");
                Serial.println(buf_wifi_password);
            }

            /**
             * Retrieve the first humidity
             * value as passed in via JSON
             */
            humidity_1 = json_doc[HUM_1_KEY];
            if(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- humidity 1 value passed in via JSON: ");
                Serial.println(humidity_1);
            
            
            /**
             * Retrieve the second humidity
             * value as passed in via JSON
             */
            humidity_2 = json_doc[HUM_2_KEY];
            if(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- humidity 2 value passed in via JSON: ");
                Serial.println(humidity_2);
            

            /**
             * Retrieve the first temperature 
             * value as passed in via JSON
             */
            temperature_1 = json_doc[TEMP_1_KEY];
            if(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- temperature 1 value passed in via JSON: ");
                Serial.println(temperature_1);
            
            
            /**
             * Retrieve the first temperature 
             * value as passed in via JSON
             */
            temperature_2 = json_doc[TEMP_2_KEY];
            if(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- temperature 2 value passed in via JSON: ");
                Serial.println(temperature_2);
            
            
            /**
             * Retrieve battery voltage  
             * value as passed in via JSON
             */
            battery_v = json_doc[BAT_V_KEY];
            if(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- battery voltage passed in via JSON: ");
                Serial.println(battery_v);
            
            
            /**
             * Retrieve battery too low
             * flag as passed in via JSON
             */
            battery_too_low = json_doc[BAT_SATUS_KEY];
            if(ENABLE_LOGGING)
                Serial.print("\tSTATE HDLR -- battery too low flag passed in via JSON: ");
                Serial.println(battery_too_low);
            
            
            current_state = NETWORK_CONNECTION;          
        
        }
        break;
        
        /**
         * @brief STATE NETWORK CONNECTION
         * 
         */
        case NETWORK_CONNECTION:
            
            WiFi.forceSleepWake();
            delay(1);                       // There are claims that a non-zero delay is required after calling the wake function

            if(!WiFiConnect(buf_wifi_ssid, buf_wifi_password)) {
                if(ENABLE_LOGGING)
                    Serial.println("Deep sleep.");
                
                current_state = DEEP_SLEEP;
            }

            current_state = SEND_EMAIL;

        break;

        /**
         * @brief SEND EMAIL STATE
         * 
         */
        case SEND_EMAIL:
        {
            AssembleEmailMessage();

            /**
             * @brief Make a client connection
             */
            if(ENABLE_LOGGING)
                Serial.println(F("Making client connection"));
            
            if (client.connect(server, 2525) == 1) {
                if(ENABLE_LOGGING)
                    Serial.println(F("connected"));
                
            } 
            else {
                if(ENABLE_LOGGING)
                    Serial.println(F("connection failed"));
                
                return;
            }
            if (!eRcv()){
                if(ENABLE_LOGGING)
                    Serial.println(F("\t*** Error connecting to the client"));
                
                FlushSerialRXBuffer( );
                current_state = DEEP_SLEEP;        
                break;
            }


            /**
             * @brief Sending EHLO command
             */
            if(ENABLE_LOGGING)
                Serial.println(F("Sending EHLO"));
            
            client.println("EHLO www.example.com");
            if (!eRcv()){
                if(ENABLE_LOGGING)
                    Serial.println(F("\t*** Error sending EHLO command."));
                
                FlushSerialRXBuffer( );
                current_state = DEEP_SLEEP;
                break;
            }
            

            /**
             * @brief Sending auth login command
             */
            if(ENABLE_LOGGING)
                Serial.println(F("Sending auth login"));
            

            client.println("auth login");
            if (!eRcv()){
                if(ENABLE_LOGGING)
                    Serial.println(F("\t*** Error sending AUTH LOGIN command."));
                
                FlushSerialRXBuffer( );
                current_state = DEEP_SLEEP;
                break;
            }

            
            /**
             * @brief Send SMTP2GO User Account Credentials
             */
            if(ENABLE_LOGGING)
                Serial.print(F("Sending SMTP2GO B64 User Account Name: "));
                Serial.println(buf_hyg_smtp2go_account);
            
            
            client.println(buf_hyg_smtp2go_account); //B64 encoded SMTP2GO username
            if (!eRcv()){
                if(ENABLE_LOGGING)
                    Serial.print(F("\t*** Error sending SMTP2GO Username: "));
                    Serial.println(buf_hyg_smtp2go_account);
                
                FlushSerialRXBuffer( );
                current_state = DEEP_SLEEP;
                break;
            }


            /**
             * @brief Send SMTP2GO Password
             * 
             * THis should be the BASE64 password 
             * for you SMTP2GO account
             */
            if(ENABLE_LOGGING)
                Serial.print(F("Sending B64 SMTP2GO Password: "));
                Serial.println(buf_hyg_smtp2go_password);
            

            client.println(buf_hyg_smtp2go_password);  
            if (!eRcv()){
                if(ENABLE_LOGGING)
                    Serial.println(F("\t*** Error sending SMTP2GO password"));
                
                FlushSerialRXBuffer( );
                current_state = DEEP_SLEEP;
                break;
            }
            

            /**
             * @brief Command for MAIL From:
             * i.e.  --> client.println(F("MAIL From: clinton.debug@gmail.com"));
             * 
             */
            memset(buf_temp, 0, TEMP_BUF_SIZE);
            strcpy(buf_temp, "MAIL From: ");
            strcat(buf_temp, buf_sender_email_addr);
            client.println(buf_temp);
            if (!eRcv()){
                if(ENABLE_LOGGING)
                    Serial.print(F("\t*** Error on command: "));
                    Serial.println(buf_temp);
                
                FlushSerialRXBuffer( );
                current_state = DEEP_SLEEP;
                break;
            }
            

            /**
             * @brief Enter recipient address
             * First, fill temp buffer with null characters
             * i.e.  -->  client.println(F("RCPT To: clinton.guenther@gmail.com"));
             * 
             */
            if(ENABLE_LOGGING)
                Serial.print(F("Sending To: "));
                Serial.println(buf_recipient_email_addr);
            
            
            memset(buf_temp, 0, TEMP_BUF_SIZE);
            strcpy(buf_temp, "RCPT To: ");
            strcat(buf_temp, buf_recipient_email_addr);
            
            client.println(buf_temp);
            if (!eRcv()){
                if(ENABLE_LOGGING)
                    Serial.print(F("\t*** Error on command: "));
                    Serial.println(buf_temp);
                
                FlushSerialRXBuffer( );
                current_state = DEEP_SLEEP;
                break;
            }
            

            /**
             * @brief Send DATA command
             */
            if(ENABLE_LOGGING)
                Serial.println(F("Sending DATA"));
            

            client.println(F("DATA"));
            if (!eRcv()){
                if(ENABLE_LOGGING)
                    Serial.println(F("\t*** Error on command \"DATA\"."));
                
                FlushSerialRXBuffer( );
                current_state = DEEP_SLEEP;
                break;
            }


            /**
             * @brief Sending To: command
             * i.e.  --> client.println(F("To: clinton.guenther@gmail.com"));
             */
            if(ENABLE_LOGGING)
                Serial.println(F("Sending email"));
            
            //  client.println(F("To:  clinton.guenther@gmail.com"));
            memset(buf_temp, 0, TEMP_BUF_SIZE);
            strcpy(buf_temp, "To: ");
            strcat(buf_temp, buf_recipient_email_addr);
            client.println(buf_temp);


            /**
             * @brief Sending From: command
             * i.e. -->  client.println(F("From: clinton.debug@gmail.com"));
             */
            // client.println(F("From: clinton.debug@gmail.com"));
            memset(buf_temp, 0, TEMP_BUF_SIZE);
            strcpy(buf_temp, "From: ");
            strcat(buf_temp, buf_sender_email_addr);
            client.println(buf_temp);


            /**
             * @brief Send the subject
             */
            client.println(F("Subject: Hygrometer Health Report\r\n"));
            client.println(email_message);
            client.println(F("."));
            if (!eRcv()){
                if(ENABLE_LOGGING)
                    Serial.println(F("\t*** Error sending DOT to complete transaction"));
                
                FlushSerialRXBuffer( );
                current_state = DEEP_SLEEP;
                break;
            }


            /**
             * @brief Sending QUIT
             * 
             */
            if(ENABLE_LOGGING)
                Serial.println(F("Sending QUIT"));
            
                        
            client.println(F("QUIT"));
            if (!eRcv()){
                if(ENABLE_LOGGING)
                    Serial.println(F("\t*** Error sending \"QUIT\"."));
                
                FlushSerialRXBuffer( );
                current_state = DEEP_SLEEP;
                break;
            }
            
            
            /**
             * @brief Disconnecting 
             */
            client.stop();
            
            if(ENABLE_LOGGING)
            Serial.println(F("disconnected"));
            

            current_state = DEEP_SLEEP;
        }
        break;

        /**
         * @brief DEEP SLEEP STATE
         */
        case DEEP_SLEEP:
            digitalWrite(WIFI_ERR_LED, LOW);
            
            if(ENABLE_LOGGING)
                    Serial.println("\t Entering into deep sleep..."); 
            

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