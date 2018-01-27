#include <Wire.h>
#include <RtcDS1307.h>
#include <EDB.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include "RDM6300.h"

SoftwareSerial rdm_serial(9, 8);
RDM6300<SoftwareSerial> rdm(&rdm_serial);
#define SD_PIN 10  // SD Card CS pin
#define TABLE_SIZE 8192

String monthString[13] = {"januari", "februari", "maret", "april", "mei", "juni", "juli", "agustus", "september", "oktober", "november", "desember"};

RtcDS1307<TwoWire> Rtc(Wire);

File dbFile;
void writer (unsigned long address, byte data) {
    digitalWrite(13, HIGH);
    dbFile.seek(address);
    dbFile.write(data);
    dbFile.flush();
    digitalWrite(13, LOW);
}

byte reader (unsigned long address) {
    digitalWrite(13, HIGH);
    dbFile.seek(address);
    byte b = dbFile.read();
    digitalWrite(13, LOW);
    return b;
}

// Create an EDB object with the appropriate write and read handlers
EDB db(&writer, &reader);

void setup () 
{
    Serial.begin(57600);     
    //setup rtc
    Rtc.Begin();
    setupRtc();
    

    //setup sdcard
    if (!SD.begin(10)) {
        Serial.println("No SD-card.");
        return false;
    }

    // Check dir for db files
    if (!SD.exists("/door-database")) {
        Serial.println("Dir for Db files does not exist, creating...");
        SD.mkdir("/door-database");
    }    
}

void loop () 
{
    if (!Rtc.IsDateTimeValid())
        Serial.println("RTC lost confidence in the DateTime!");

    RtcDateTime dt = Rtc.GetDateTime();
    String db_name = "/door-database/file-"+monthString[dt.Month()]+"-"+dt.Year()+".db";
    
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void getStringDate(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
   return datestring;
}

void setupRtc(){
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    if (!Rtc.IsDateTimeValid())
        Rtc.SetDateTime(compiled);
    if (!Rtc.GetIsRunning())
        Rtc.SetIsRunning(true);
        
    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
        Rtc.SetDateTime(compiled);
        
    Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low); 
}


