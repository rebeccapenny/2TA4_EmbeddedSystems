#include "mbed.h"
 
InterruptIn userButton(BUTTON1);
 
DigitalOut led3(PG_13);
DigitalOut led4(PG_14);
Ticker ticker1;
Ticker ticker2;
Ticker ticker3; // ticker for state 2
Ticker ticker4; // ticker for state 2
Timeout timeoutDelay;
Timeout timeoutDelay2;
Timeout timeoutDelay3;
 
 
int state = 0; // default state on startup
 
void ticker1ISR(){
    led3 = !led3;
}
 
void ticker2ISR(){
    led4 = !led4;
}
 
void ToggleRedOn(){
    led4 = 1;
}
 
void ToggleRedOff(){
    led4 = 0;
}
 
void ToggleGreenOn(){
    led3 = 1;
}
 
void ToggleGreenOff(){
    led3 = 0;
}
 
void Delay1ISR(){
    ticker1.attach(&ticker1ISR, 1s);
}
 
void Delay2ISR(){
    ticker2.attach(&ticker2ISR, 1s);
}
 
void OffsetRedISR(){
    ticker2.attach(&ToggleRedOff, 4s);
}
 
void DelayGreenOffISR(){
    ticker4.attach(&ToggleGreenOff, 4s);
}
 
void OffsetGreenISR(){
    ticker3.attach(&ToggleGreenOn, 4s);
    timeoutDelay3.attach(&DelayGreenOffISR, 1s);
}
 
 
void SetState() {
  if (state != 1) {
    state = 1;
    led3 = 0;
    led4 = 1;
    ticker1.detach();
    ticker2.detach();
    ticker3.detach();
    ticker4.detach();
    timeoutDelay.attach(&Delay1ISR, 2000ms);
    ticker2.attach(&ticker2ISR, 1s);
  }

  // 4 tickers used for state 2 was inspired by Braeden Marchant
  else {
    state = 2;
    led3 = 0;
    led4 = 1;
    ticker1.detach();
    ticker2.detach();
    ticker3.detach();
    ticker4.detach();
    ticker1.attach(&ToggleRedOn, 4s);
    timeoutDelay.attach(&OffsetRedISR, 1s);
    timeoutDelay2.attach(&OffsetGreenISR, 2s);
  }
}
 
int main(){
    userButton.fall(&SetState);
    __enable_irq();
    led3 = 0;
    led4 = 0;
    ticker2.attach(&ticker2ISR,1s);
    while(true){
    }
}
