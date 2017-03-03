#include <Arduino.h>

#include <menu.h>
#include <menuIO/serialOut.h>

using namespace Menu;

int test=55;

//custom field edit
//implementing a customized menu component
//this components can be latter shared as plugins
template<typename T>
class cancelField:public menuField<T> {
protected:
  T original;//to use when canceling
  bool editing;
public:
  cancelField(const menuFieldShadow<T>& shadow):menuField<T>(shadow),editing(false) {}
  void doNav(navNode& nav,navCmd cmd) override {
    Serial<<endl<<"cmd:"<<cmd;
    if (!editing) {
      original=target();
      editing=true;
    }
    switch(cmd.cmd) {
      case escCmd:
        editing=false;
        target()=original;
        menuField<T>::tunning=true;
        break;
      case enterCmd:
        if (menuField<T>::tunning||options->nav2D||!tune())
          editing=false;
        break;
      default: break;
    }
    menuField<T>::doNav(nav,cmd);
  }
};

//a menu using a customized components
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,altFIELD(cancelField,test,"Custom edit","%",0,100,10,1,doNothing,enterEvent,wrapStyle)
  ,FIELD(test,"Original edit","%",0,100,10,1,doNothing,noEvent,wrapStyle)
);

#define MAX_DEPTH 1

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"menu 3.0 testing custom fields"<<endl;Serial.flush();
}

void loop() {
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}