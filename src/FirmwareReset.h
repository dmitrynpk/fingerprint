// The Firmware reset library creates a small window of time, in which the reset button can be pressed to initiate a firmware reset.
// The state of the window is displayed to the user by flashing the built in LED or by Serial updates.

#ifndef FirmwareReset_H
#define FirmwareReset_H

#define FLAGSET     0x55555555
#define FLAGCLEAR   0xAAAAAAAA
#define FLAGADDRESS 00
#define SETTIMEOUT 3
#define CLEARTIMEOUT 3

bool booted = false;

// End the firmware reset process, at this point the software can run normally without checking for user resets
void finishBoot() {
  booted = true;
  
  DEBUG_MSGF("Booted\n");

  ticker.detach();
}

// Clear the reset flag in memory, this ends the window for a possible reset
void clearFlag() {
  uint32_t value = FLAGCLEAR;
  ESP.rtcUserMemoryWrite(FLAGADDRESS, &value, sizeof(value));

  DEBUG_MSGF("Stop waiting for reset\n");

  ticker.attach(CLEARTIMEOUT, finishBoot);
}

// Set the reset flag in memory, this will indicate if the user has pressed reset while the LED was on
void setFlag() {
  uint32_t value = FLAGSET;
  ESP.rtcUserMemoryWrite(FLAGADDRESS, &value, sizeof(value));
  
  DEBUG_MSGF("Start waiting for reset\n");

  ticker.attach(CLEARTIMEOUT, clearFlag);
}

// Checks to see if the user has rebooted the processor within the reset time frame
bool checkResetFlag() {
  uint32_t value = FLAGCLEAR;
  bool result;
  
  // Check to see the state of the flag in memory
  ESP.rtcUserMemoryRead(FLAGADDRESS, &value, sizeof(value));  
  result = (value == FLAGSET);

  // If the user previously performed a reset then begin the reset process
  if(result) 
    clearFlag();

  // If the flag was not previously set, then set up a new reset window
  else  
    ticker.attach(SETTIMEOUT, setFlag);

  return result;
}


#endif
