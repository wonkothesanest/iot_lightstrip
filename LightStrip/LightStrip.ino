#include <Servo.h>


//Libraries
#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <HSBColor.h>

//Item identification: must be different for each light
const String itemName = "dustin_bedroom_desk_light";

//Network information
const char *ssid = "ASUS";
const char *pass = "alamoana";
const char *mqtt_server = "osmc";

//MQTT topics
const String mqttInputOnOffCommand = "openhab/out/" + itemName + "/command";
const String mqttInputColorCommand = "openhab/out/" + itemName + "_color/command";
const String mqttOutputOnOffState = "openhab/in/" + itemName + "/state";
const String mqttOutputColorState = "openhab/in/" + itemName + "_color/state";
const String mqttOutputMotionState = "openhab/in/" + itemName + "_motion/state";


//Pins
#define PRED 12
#define PGREEN 14
#define PBLUE 13
#define pBtnColor 15
#define pBtnOnOff 2
#define pMotionDetector 0
#define pSonarPing 4
#define pSonarPong 4

//Variables for state
//Actual pwm max is 1023, for power consumption we limit to:
const uint16_t pwm_max = 900;
volatile int last_rgb[3] = {pwm_max, pwm_max, pwm_max};
volatile int btn_onoff_state = 0;
volatile int btn_color_state = 0;
volatile bool is_on = false;
volatile unsigned long lastBtnPress = 0;
const unsigned long BtbDebounceTime = 50;
volatile int lastColorSelectedIndex = -1;

//List of static colors
const int numColors = 7;
const float colors[numColors][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}};

WiFiClient net;
MQTTClient client;

void connect(); // <- predefine connect() for setup()

void setup() {
  Serial.begin(9600);
  Serial.println("Starting ESP module\n");
  WiFi.begin(ssid, pass);
  client.begin(mqtt_server, net);

  pinMode(PRED, OUTPUT);
  pinMode(PGREEN, OUTPUT);
  pinMode(PBLUE, OUTPUT);

  pinMode(pBtnOnOff, INPUT);
  pinMode(pBtnColor, INPUT);
  pinMode(pMotionDetector, INPUT);

  //Interrupt for Buttons
  attachInterrupt(digitalPinToInterrupt(pBtnColor), pin_ISR_COLOR, FALLING);
  attachInterrupt(digitalPinToInterrupt(pBtnOnOff), pin_ISR_ONOFF, FALLING);

  //Interrupt for Motion detector
  attachInterrupt(digitalPinToInterrupt(pMotionDetector), pin_ISR_MOTION, RISING);

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

  //Subscribe to commands coming from Openhab for changes in color
  client.subscribe(mqttInputOnOffCommand);
  client.subscribe(mqttInputColorCommand);
}

void loop() {
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if(!client.connected()) {
    connect();
  }

  //Delay a minute?
  delay(10000);

}

//uses valuue of last_rgb
void changeLEDs(){
  analogWrite(PRED, last_rgb[0]);
  analogWrite(PGREEN, last_rgb[1]);
  analogWrite(PBLUE, last_rgb[2]);

  //convert to HSB
  int hsb[3];
  H2R_RGBtoHSB((int*)&last_rgb,  (int*)&hsb);
  String s = String(hsb[0]) + "," + String(hsb[1]) + "," + String(hsb[2]);
  String rgbstring =  String(last_rgb[0]) + ", " +  String(last_rgb[1]) + ", " +  String(last_rgb[2]);
  Serial.println("In change LEDs: \n   RGB: " + rgbstring + "\n   HSB: " + s);
  client.publish(mqttOutputColorState, s);
  client.publish(mqttOutputOnOffState, "ON");
  is_on = true;
}

void turnOffLight(){
  analogWrite(PRED, 0);
  analogWrite(PGREEN, 0);
  analogWrite(PBLUE, 0);
  Serial.println("Turned the light off");
  client.publish(mqttOutputOnOffState, "OFF");
  is_on = false;

}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();
  if(topic == mqttInputColorCommand){
    if(payload == "0"){
      Serial.println("Turning light off");
      turnOffLight();

    }else if(payload == "1"){
      Serial.println("Turning Light On");
      changeLEDs();
    }else{
      Serial.println("I really dont know what to do");
    }
  }else if(topic==mqttInputColorCommand){
    String value=payload;
    int SoffitR = value.substring(0,value.indexOf(',')).toInt();
    int SoffitG = value.substring(value.indexOf(',')+1,value.lastIndexOf(',')).toInt();
    int SoffitB = value.substring(value.lastIndexOf(',')+1).toInt();
    H2R_HSBtoRGB(SoffitR, SoffitG, SoffitB, (int*)&last_rgb);
    Serial.print("R ");
    Serial.println(last_rgb[0]);
    Serial.print("G ");
    Serial.println(last_rgb[1]);
    Serial.print("B ");
    Serial.println(last_rgb[2]);
    changeLEDs();
  }
}

void pin_ISR_COLOR() {

  //Change color
  if(millis() >= lastBtnPress + BtbDebounceTime){
    if(lastColorSelectedIndex<0 || lastColorSelectedIndex >= numColors - 1){
      lastColorSelectedIndex = 0;
    }else{
      lastColorSelectedIndex++;
    }

    last_rgb[0] = (int)colors[lastColorSelectedIndex][0];
    last_rgb[1] = (int)colors[lastColorSelectedIndex][1];
    last_rgb[2] = (int)colors[lastColorSelectedIndex][2];
    changeLEDs();

  }

  lastBtnPress = millis();
}

void pin_ISR_ONOFF() {

  //Change state
  if(millis() >= lastBtnPress + BtbDebounceTime){
    if(is_on){
      turnOffLight();
    }else{
      changeLEDs();
    }
  }

  lastBtnPress = millis();
}

void pin_ISR_MOTION(){
  Serial.println("Motion detected");
  client.publish(mqttOutputMotionState, "ON");
}
