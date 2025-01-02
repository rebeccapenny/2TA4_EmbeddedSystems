#include "LCD_DISCO_F429ZI.h"
#include "mbed.h"
#include <ctime>
#include <chrono>
 
LCD_DISCO_F429ZI LCD;
InterruptIn userButton(BUTTON1);
DigitalOut led1(LED1);
Timer timer1;
Ticker ticker;
Timeout timeoutDelay;
InterruptIn external_button(PA_6, PullUp);
 
bool timer = false; // not timing
uint32_t reaction_time; // current time
uint32_t best_time; // best reaction time
 
bool timing = false;
int state = 1;
int print_results = 0;
int counttimes = 0;
bool print1 = false;
bool print2 = false;
bool print3 = false;
 
void led1ISR(){
    led1 = !led1;
}
 
void led1On(){
    led1 = 1;
    timer1.start();
}
 
void UserButtonTimer() {
    timer1.stop();
    reaction_time =
        chrono::duration_cast<chrono::milliseconds>(timer1.elapsed_time())
            .count();
    if (counttimes == 0 || reaction_time < best_time) {
      best_time = reaction_time;
      counttimes=1;
    }
    print_results = 0;
    timer1.reset();
    led1 = 0;
}
 
 
void SetState(){
    if (state == 0){
        print1 = false;
        print2 = false;
        print3 = false;
        ticker.attach(&led1ISR,100ms);
        state = 1;
    }
    
    else if (state == 1) {
        print_results = 0;
        print1 = false;
        print2 = false;
        print3 = false;
        ticker.detach();
        timer1.reset();
        state = 2;
        led1 = 0;
        int randomDelay = rand()% 4000 + 1000;
        timeoutDelay.attach(&led1On, std::chrono::milliseconds(randomDelay));
    }
    else if (state == 2){
        state = 0;
        timer1.stop();
        timeoutDelay.detach();
        led1 = 0;
        reaction_time = chrono::duration_cast<chrono::milliseconds>(timer1.elapsed_time()).count();
        timer1.reset();
 
        if (reaction_time == 0){
            print3 = true;
            print1 = false;
            timer1.reset();
        }
 
        else if (counttimes == 0 || (reaction_time < best_time && reaction_time > 0)) {
            best_time = reaction_time;
            counttimes = 1;
            print3 = false;
        }
        print1 = true;
    }
}
 
void resetISR(){
    timer1.stop();
    timer1.reset();
    state = 1;
    print1 = false;
    print3 = false;
    timeoutDelay.detach();
    ticker.detach();
    ticker.attach(&led1ISR,100ms);
    best_time = 0;
    counttimes = 0;
}
 
 
int main() {
    LCD.SetFont(&Font24);
    LCD.SetTextColor(LCD_COLOR_DARKBLUE);
    srand (time(NULL));
    userButton.fall(&SetState);
    external_button.fall(&resetISR);
    __enable_irq();
    ticker.attach(&led1ISR,100ms);
 
 
    
 
    while (1) {
        if (print1){
            LCD.Clear(LCD_COLOR_WHITE);
            uint8_t text[30];
            LCD.DisplayStringAt(0, 40, (uint8_t *)"Reaction Times", LEFT_MODE);
            sprintf((char *)text, "Latest: %d ms", reaction_time);
            LCD.DisplayStringAt(0, 80, (uint8_t *)&text, LEFT_MODE);
            sprintf((char *)text, "Best: %d ms", best_time);
            LCD.DisplayStringAt(0, 120, (uint8_t *)&text, LEFT_MODE);
            thread_sleep_for(1000);
        }
        else if (print3){
            LCD.Clear(LCD_COLOR_WHITE);
            uint8_t text[30];
            LCD.DisplayStringAt(0, 40, (uint8_t *)"Try Again", LEFT_MODE);
            thread_sleep_for(1000);           
        }
        else {
            LCD.Clear(LCD_COLOR_WHITE);
            thread_sleep_for(1000);
        }
    }
}