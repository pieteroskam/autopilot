#ifndef button_h
#define button_h

class Button
{
  private:
    uint8_t btn;
    unsigned long lastDebounceTime;
    int lastSteadyState;
    int lastFlickerableState;
    int currentState;
  public:
    void begin(uint8_t button) {
      btn = button;      
      unsigned long lastDebounceTime = 0;
      lastSteadyState = HIGH;
      lastFlickerableState = HIGH;
      currentState = HIGH;
      pinMode(btn, INPUT_PULLUP);
    }
    
    bool debounce() {
      // read the state of the switch/button:
      currentState = digitalRead(btn);
    
      // check to see if you just pressed the button
      // (i.e. the input went from LOW2 to LOW), and you've waited long enough
      // since the last press to ignore any noise:
    
      // If the switch/button changed, due to noise or pressing:
      if (currentState != lastFlickerableState) {
        // reset the debouncing timer
        lastDebounceTime = millis();
        // save the the last flickerable state
        lastFlickerableState = currentState;
      }
    
      if ((millis() - lastDebounceTime) > 50) {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:
    
        // if the button state has changed:
        if(lastSteadyState == LOW && currentState == HIGH){
          lastSteadyState = currentState;
          
          Serial.print(btn);
          Serial.println(" button pressed");
          return true;
        }else if(lastSteadyState == HIGH && currentState == LOW){
          lastSteadyState = currentState;
          Serial.print(btn);
          Serial.println(" button released");
        }
      }
      return false;
    }
};
#endif


Button btn_min_10;
Button btn_min_1;
Button btn_plus_10;
Button btn_plus_1;

Button btn_mode;
Button btn_on;

void button_setup(){

  btn_on.begin(25);
  btn_min_10.begin(26);
  btn_min_1.begin(27);
  btn_plus_10.begin(12);
  btn_plus_1.begin(14);
  btn_mode.begin(13);
  
  

}

void button_loop(){
   if (btn_plus_10.debounce()) {
      Serial.println("BUTTON PLUS 10 PRESSED");
      change_heading_command(10);
  }
  if (btn_plus_1.debounce()) {
      Serial.println("btn_plus_1 PRESSED");
      change_heading_command(1);
  }
  
  if (btn_min_10.debounce()) {
      Serial.println("btn_min_10  PRESSED");
      change_heading_command(-10);
  }
  if (btn_min_1.debounce()) {
      Serial.println("btn_min_1 PRESSED");
      change_heading_command(-1);
  }
  if (btn_mode.debounce()) {
      Serial.println("btn_mode PRESSED");
      mode_selected++;
      if(mode_selected==5){
        mode_selected=0;
      }
  }
  if (btn_on.debounce()) {
      Serial.println("btn_on PRESSED");
      apToggle();      
  }
  
}
