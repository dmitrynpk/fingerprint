// Creates an admin page on a webserver which allows the user to update the SSID and Password
// Performs basic checks to ensure that the input values are valid

#ifndef AdminPage_h
#define AdminPage_h

//Holds the admin webpage in the program memory
const char adminPage[] PROGMEM = 
    "<html>"
      "<head>"
        "<style>input {font-size: 1.2em; width: 100%; max-width: 350px; display: block; margin: 5px auto; } </style>"
      "</head>"
      "<body>"
        "<form id='form' action='/' method='post'>"
          "<input name='newssid' type='text' placeholder='New SSID'>"
          "<input name='newpassword' id='text' type='password' placeholder='New Password'>"
          "<input type='submit' value='Update'>"
        "</form>"
      "</body>"
    "</html>";


//Creates a webpage that allows the user to change the SSID and Password from the browser
void serverAdmin(ESP8266WebServer *webServer) {
  String message;  
  
  // Check to see if we've been sent any arguments and instantly return if not
  if(webServer->args() == 0) {
    
    webServer->sendHeader("Content-Length", String(strlen(adminPage)));
    webServer->send(200, "text/html", adminPage);
  }
  else {      
      
    // Create a string containing all the arguments, send them out to the serial port
    // Check to see if there are new values (also doubles to check the length of the new value is long enough)
    DEBUG_MSG(webServer->arg("newssid"));
    
    if((webServer->arg("newssid").length() >= MIN_STR_LEN) &&
       (webServer->arg("newssid").length() < MAX_STR_LEN)) 
      webServer->arg("newssid").toCharArray(config.ssid, sizeof(config.ssid));

    if(webServer->arg("newpassword").length() < MAX_STR_LEN) 
      webServer->arg("newpassword").toCharArray(config.pass, sizeof(config.pass));
    
    // Store the new settings to EEPROM
    SaveConfig();
    PrintConfig();

    // Construct a message to tell the user that the change worked
    message = "New settings will take effect after restart";     

    // Reply with a web page to indicate success or failure
    message = "<html><head><meta http-equiv='refresh' content='5;url=/' /></head><body>" + message;
    message += "<br/>Redirecting in 5 seconds...</body></html>";
    webServer->sendHeader("Content-Length", String(message.length()));
    webServer->send(200, "text/html", message);
  }
}
#endif
    
