/*
    EMduino.cpp - Library for arduino EMduino
    Author: Brian D. Arpie
    Date Created: 4/6/2014
    Date Updated: 4/21/2014
*/

#include "EMduino.h"

#define PI 3.14159265359    
#define SIZE_OF_PACKAGE 32
#define SLEEP_TIME 5
#define MAX_COUNT 15
#define MAX_DELAY 15

uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

RF24 radio( 9, 10 );

/**
*   constructor EMduino
*   initalize variables and do setup
**/
EMduino::EMduino( String _cmd )
{
   cmd = _cmd;
   isReceiving = false;
   isSending = false;
   isStringComplete = false;
   isDataReceived = false;
   send = "send";
   receive = "receive";
   _dataPacket = "";
   _sendPackage[SIZE_OF_PACKAGE];
   _receivePackage[SIZE_OF_PACKAGE];
   trigPin = 7,
   echoPin = 8;
  setMode();

}

/**
*   void EMduino::setMode();
*   two options: send or receive
**/
void EMduino::setMode()
{
  if( cmd == send ) { 
    isReceiving = false;
    isSending = true;
  } else if( cmd == receive ) {
    isReceiving = true;
    isSending = false;
  } else {
    Serial.print( "invalid parameter passed: " );
    Serial.print( cmd );
    Serial.println( "please pass the string 'send' or 'receive' to the EMduino constructor..." );
    isReceiving = false;
    isSending = false;
  }
}

/**
*   void EMduino::setupRadio()
*   check for receive or send -> initalize radio
**/
void EMduino::setupRadio()
{
  if( isReceiving ) {
    radio.begin();
    radio.setRetries( MAX_DELAY, MAX_COUNT ); 
    radio.setPayloadSize( SIZE_OF_PACKAGE );
    radio.openWritingPipe( pipes[1] );
    radio.openReadingPipe( 1, pipes[0] );
    radio.startListening();
  } else if ( isSending ) {
    radio.begin();
    radio.powerDown();
    radio.setRetries( MAX_DELAY, MAX_COUNT );
    radio.setPayloadSize( SIZE_OF_PACKAGE );
  }
}

/**
*   void EMduino::setPacketContents()
*   set the content of the packet being sent across radio transmission
**/
void EMduino::setPacketContent(double _data)
{
  int decimalPlaces = 2;
  if( decimalPlaces != 0 ) {
  data = String( ( int )( _data * pow( 10, decimalPlaces ) ) );
  if( abs(_data) < 1 ) {
    if( _data > 0 )
      data = "0" + data;
    else if( _data < 0 )
      data = data.substring( 0, 1 ) + "0" + data.substring( 1 );
    }
    data =  data.substring( 0, data.length() - decimalPlaces ) + "." + data.substring( data.length() - decimalPlaces );
  }
  else {
    data = String( ( int ) _data );
  }

}

/** 
*   void EMduino::read()
*   receive data -> delay 100ms -> process data and print
**/
void EMduino::read()
{   
  if ( isReceiving ) {
    if( radio.available() ) {
      bool done = false;
      while( !done ) {
          done = radio.read( &_receivePackage, SIZE_OF_PACKAGE );
          delay(5);
      }
      radio.stopListening();
      _dataPacket = ((char *)_receivePackage);
      isStringComplete = true;
      radio.write( "1", 1 );
      radio.startListening();
    }

    if( isStringComplete ) {
      if( _dataPacket ) {
          Serial.print( _dataPacket );
          Serial.print(" mL");
          Serial.print( " @ " );
          Serial.print( millis() );
          Serial.println(" ms");
      }
      _dataPacket = "";
      isStringComplete = false;
    }
  }
}

/**
*   void EMduino::write()
*   start writing
**/
void EMduino::write()
{
  if( isSending ) {
    if( !isDataReceived ) {
      radio.powerUp();
      if( !data ) {
        data = "nothing to transmit.";
      }
      isDataReceived = true;
      data.getBytes( _sendPackage, SIZE_OF_PACKAGE );
      radio.stopListening();
      radio.openWritingPipe( pipes[0] );
      radio.openReadingPipe( 1, pipes[1]);
      bool ok = radio.write( _sendPackage, SIZE_OF_PACKAGE );
      if( !ok ) {
        isDataReceived = false;
      }
      radio.startListening();
      unsigned long started_waiting_at = millis();
      boolean timeout = false;
      while( !radio.available() && !timeout ) {
        if( millis() - started_waiting_at > 500 ) {
          timeout = true;
        }
      }
      if( timeout ) {
        isDataReceived = false;
        Narcoleptic.delay( SLEEP_TIME );
      }
      radio.openWritingPipe( pipes[1] );
      radio.openReadingPipe( 1, pipes[0] );
      radio.stopListening(); 
      radio.powerDown();
    }
  }
}