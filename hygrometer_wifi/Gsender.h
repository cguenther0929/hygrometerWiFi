/* Gsender class helps send e-mails from Gmail account
*  using Arduino core for ESP8266 WiFi chip
*  by Boris Shobat
*  September 29 2016
*/
#ifndef G_SENDER
#define G_SENDER
#define GS_SERIAL_LOG_1         // Print to Serial only server responce
//#define GS_SERIAL_LOG_2       //  Print to Serial client commands and server responce
#include <WiFiClientSecure.h>

class Gsender
{
    protected:
        Gsender();
    private:
        const int SMTP_PORT = 465;
        const char* SMTP_SERVER = "smtp.gmail.com";
        
        // const char* hyg_email_addr_base64 = val_passed_in;   //Base64 format of email address
        // const char* hyg_email_pass_base64 = buf_hyg_email_password_b64;
        // const char* FROM = buf_hyg_email_address;
        
        
        /**
         * TODO: cleanup stale code
         */
        // const char* hyg_email_addr_base64 = "Y2xpbnRvbi5kZWJ1Z0BnbWFpbC5jb20=";   //Base64 format of clinton.debug@gmail.com  -- use base64encode.org to create
        // const char* hyg_email_pass_base64 = "TUdINTY3JiooNDI0MjYy";                 //Base64 format of password -- use base64encode.org to create
        // const char* FROM = "clinton.debug@gmail.com";
        
        char hyg_email_addr_base64[64];
        char hyg_email_pass_base64[64];
        char hyg_email_addr_decoded[32];           //Not base64 encoded
        
        const char* _error = nullptr;
        char* _subject = nullptr;
        String _serverResponce;
        static Gsender* _instance;
        bool AwaitSMTPResponse(WiFiClientSecure &client, const String &resp = "", uint16_t timeOut = 10000);

    public:
        static Gsender* Instance();
        Gsender* Subject(const char* subject);
        Gsender* Subject(const String &subject);

        /**
         * TODO:  CJG TEST CODE
         */
        bool SetEmailBase64(const char* val);
        bool SetPassword(const char* val);                      //TODO TODO Added by CJG.  This, and the function in .cpp works!!! Now carry through for email address / etc. 
        bool SetEmailDecoded(const char* val);

        /**
         * TODO: END CJG TEST CODE
         */

        
        bool Send(const String &to, const String &message);
        String getLastResponce();
        const char* getError();
};
#endif // G_SENDER
