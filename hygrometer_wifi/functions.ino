/* Setup initial state conditions */
bool WiFiConnect( const char * ssid, const char * password ) {
    int i = 0;
    int timeout = (int)WIFI_CONNECT_TIMEOUT_S/0.5;
    
    
    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED && i < timeout) {
        delay(500);
        // Serial.print(".");
    }

    if(i == timeout){
        return false;
    }

    return true;

    // Serial.println("");
    // Serial.print("Connected to ");
    // Serial.println(ssid);
    // Serial.print("IP address: ");
    // Serial.println(WiFi.localIP());
  
}

void FlushSerialRXBuffer( void ){
    char rxchar;    
    
    while(Serial.available() > 0) {   //Flush the buffer 
        rxchar = Serial.read();
    }
}

void SerialReadtoArray ( void ) {
    int i;
    char rxchar;
    int timeout;
    
    memset(&data_input_string,'\0', MAX_RX_BUF_ELEMENTS);

    // while (!Serial.available()) // TODO we entered this function on the basis of there being data
    //     delay(10);
    // }  

    rxchar = '\0';
    i = 0;
    timeout = 0;

    while(rxchar != '}') {

        if(Serial.available()) {
            rxchar = Serial.read();
            data_input_string[i] = rxchar;
            i++;
            
            if(i >= MAX_RX_BUF_INDEX) {
                break;
            }

        }
        else{
            timeout++;
            delay(1);
            if(timeout == SER_TIMEOUT_MS) {
                break;
            }

        }

    }

    if(timeout == SER_TIMEOUT_MS) {
        /**
         * TODO: how to indicate this 
         * error
         */
        delay(1);
        // Serial.println("Timout!");
    }
    
    FlushSerialRXBuffer();
    /**
     * TODO: cleanup
     */
    // Serial.print("Received string: "); Serial.println(data_input_string);


}