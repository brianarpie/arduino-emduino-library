/*
 EMduino.h - Library for arduino EMduino
 Author: Brian D. Arpie
 Date Created: 3/30/2014
 Date Updated: 4/21/2014
*/

 #ifndef EMduino_h
 #define EMduino_h

 #include "Arduino.h"
 #include "RF24.h"
 #include "Narcoleptic.h"
 #include "Volume.h"

 class EMduino
 {
  public:
     EMduino(String _cmd); // constructor
     void setMode();
     void setupRadio();
     void read();
     void write();
     void setPacketContent(double _data);

   
  private:
     byte _sendPackage[];
     byte _receivePackage[];
     String cmd;
     String send;
     String receive;
     String _dataPacket; 
     String shape;
     String data;
     bool isSending;
     bool isReceiving;
     bool isStringComplete;
     bool isDataReceived;
     double volume;
     double length;
     double width;
     double height;
     double radius;
     double distance;
     double duration;
     int trigPin;
     int echoPin;
  };

 #endif