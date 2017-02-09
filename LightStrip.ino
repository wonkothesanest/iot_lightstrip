// This example uses an Adafruit Huzzah ESP8266
// to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <HSBColor.h>

const char *ssid = "ASUS";
const char *pass = "alamoana";

#define PRED 12
#define PGREEN 14
#define PBLUE 13

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect(); // <- predefine connect() for setup()

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  client.begin("192.168.1.198", net);
  
  pinMode(PRED, OUTPUT);
  pinMode(PGREEN, OUTPUT);
  pinMode(PBLUE, OUTPUT);


  connect();
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "try", "try")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/example");
  client.subscribe("light/cmd");
  client.subscribe("light/color");
  // client.unsubscribe("/example");
}

void loop() {
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if(!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if(millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/hello", "world");
  }
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();
  if(topic == "light/cmd"){
    if(payload == "0"){
      Serial.println("Turning light off");
      analogWrite(PRED, 0);
      analogWrite(PGREEN, 0);
      analogWrite(PBLUE, 0);
      client.publish("light/state", "OFF");
    }else if(payload == "1"){
      Serial.println("Turning Light On");
      analogWrite(PRED, 1000);
      analogWrite(PGREEN, 1000);
      analogWrite(PBLUE, 1000);
      client.publish("light/state", "ON");
    }else{
      Serial.println("I really dont know what to do");
    }
  }else if(topic=="light/color"){
    String value=payload;
    int SoffitR = value.substring(0,value.indexOf(',')).toInt();
    int SoffitG = value.substring(value.indexOf(',')+1,value.lastIndexOf(',')).toInt();
    int SoffitB = value.substring(value.lastIndexOf(',')+1).toInt();
    int rgb[3];
    H2R_HSBtoRGB(SoffitR, SoffitG, SoffitB, (int*)&rgb);
    Serial.print("R ");
    Serial.println(rgb[0]);
    Serial.print("G ");
    Serial.println(rgb[1]);
    Serial.print("B ");
    Serial.println(rgb[2]);
    analogWrite(PRED, rgb[0]);
    analogWrite(PGREEN, rgb[1]);
    analogWrite(PBLUE, rgb[2]);
  }
}
