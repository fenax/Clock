#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>
#include <SoftwareSerial.h>


SoftwareSerial swSer1(-1,D2,false);
const char *ssid     = "FRITZ!Box 7530 ZH";
const char *password = "69072508777412404252";

const char segment = 0x11;
const char led     = 0x12;
const char bright  = 0x13;

WiFiUDP ntpUDP;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600*2, 60000);

void setup(){
  pinMode(D5,INPUT);
  pinMode(D6,INPUT);
  pinMode(D7,INPUT);
//  pinMode(D8,INPUT);
//  digitalWrite(D5,HIGH);
//  digitalWrite(D6,HIGH);
 // digitalWrite(D7,HIGH);
//  digitalWrite(D8,HIGH);

//pinMode(D4,INPUT);
digitalWrite(D4,LOW);
  Serial.begin(115200);

  pinMode(D3,OUTPUT);
  digitalWrite(D3,HIGH);//disable clock relay
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  timeClient.begin();
  
  digitalWrite(D3,LOW);//enable clock relay
    
  delay(100);
  Serial1.begin(115200);
  set_brightness(0x88);
}


void send(char value){
  Serial1.write(value);
 // Serial.printf("write %x %c\n",value,value);
}

static char bri = 0x88;

void cycle_brightness(){
  if (bri==0x90) bri = 0x88;
Serial.println("brightness ");
Serial.printf("%d\n",bri);
set_brightness(bri);
bri++;
}

void set_brightness(char value){
  send(0x13);   send(value); 
  delay(10);
  send('1');
  delay(10);
  send('2');
  delay(10);
  send('3');
  delay(10);
  send('4');
  delay(10);
  send('5');
  delay(10);
}

void read_dipswitch(){
  if (digitalRead(D7)){
//    Serial.println("winter time");
    timeClient.setTimeOffset(3600);
  }else{
//    Serial.println("summer time");
    timeClient.setTimeOffset(3600*2);
  }
}

void update_brightness(){
  static bool previous = true;
  if (digitalRead(D6)){
    if (!previous){
      set_brightness(0x88);
      previous = true;
    }
  }else{
    if(previous){
      set_brightness(0x8f);
      previous = false;
    }
  }
}

void print_segments(char a, char b){
  Serial1.write(0x11);
  Serial1.write(a);
  Serial1.write(b);
}

void loop() {
  bool rc;
  read_dipswitch();
  update_brightness();


  rc = timeClient.update();
  auto t = timeClient.getFormattedTime();
 // Serial.println(t);
  if (rc) {
    t[2] = ':'; 
  }else{
    t[2] = '-';
  }
  for (int i=0;i<5;i++){
    send(t[4-i]);
  }
//print_segments(0xff,0xff);
//print_segments(0,0x33);


  delay(1000);
  }
