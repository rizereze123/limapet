#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>

const char* ssid = "Ras";
const char* password = "iyasokweh";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27,16,2);
Servo servo;
Servo servo2;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
#define servo1 D3
//#define servo2 D4
#define buzzer D0
#define capacity1_Trig_Ultrasonic D6
#define capacity1_Echo_Ultrasonic D5
#define capacity2_Trig_Ultrasonic D8
#define capacity2_Echo_Ultrasonic D7

int CLOSE_ANGLE = 0;  
int OPEN_ANGLE = 90; 

int CLOSE_ANGLE_2 = 0;  
int OPEN_ANGLE_2 = 55; 

int dosis;

void setup_wifi(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for(int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if((char)payload[0] == '2'){
      dosis = 2000;
  }
  if((char)payload[0] == '3'){
      dosis = 3000;
  }
  if((char)payload[0] == '4'){
      dosis = 4000;
  }
  if((char)payload[0] == '5'){
      dosis = 5000;
  }
  if((char)payload[0] == '6'){
      dosis = 6000;
  }
  if((char)payload[0] == '7'){
      dosis = 7000;
  }
  if((char)payload[0] == '8'){
      dosis = 8000;
  }
  if((char)payload[0] == '9'){
      dosis = 9000;
  }
  if((char)payload[0] == '0'){
    open_feed();
    delay(dosis);
    close_feed();
  }
  if((char)payload[0] == '1'){
    open_feed_2();
    delay(dosis);
    close_feed_2();
  }
  
}

void reconnect(){
  // Loop until we're reconnected
  while (!client.connected()){
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if(client.connect(clientId.c_str())){
      Serial.println("connected");
      client.subscribe("limapet");
      } 
    else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      }
   }
}

void open_feed(){
  servo.write(OPEN_ANGLE); 
  digitalWrite(buzzer, HIGH);
}

void close_feed(){
  servo.write(CLOSE_ANGLE);   
  digitalWrite(buzzer, LOW);
}

void open_feed_2(){
  servo2.write(OPEN_ANGLE_2); 
  digitalWrite(buzzer, HIGH);
}

void close_feed_2(){
  servo2.write(CLOSE_ANGLE_2);   
  digitalWrite(buzzer, LOW);
}

void displaylcd(){
    lcd.setCursor(0,0);
    lcd.print("SELAMAT DATANG");
    lcd.setCursor(0,1);
    lcd.print("DI LIMAPET!!");
}

void capacityServo1(){
  digitalWrite(capacity1_Trig_Ultrasonic, LOW); 
  delayMicroseconds(2); 
  digitalWrite(capacity1_Trig_Ultrasonic, HIGH); 
  delayMicroseconds(10); 
  int duration = pulseIn(capacity1_Echo_Ultrasonic, HIGH); 
  int jarak = duration*0.034/2;

  if(jarak < 5){
    client.publish("ultrasonic1", "100");
  }
  if(jarak > 5 && jarak < 10){
    client.publish("ultrasonic1", "75");
  }
  if(jarak > 10 && jarak < 15){
    client.publish("ultrasonic1", "50");
  }
  if(jarak > 15 && jarak < 20){
    client.publish("ultrasonic1", "25");
  }
  if(jarak > 20 && jarak < 25){
    client.publish("ultrasonic1", "10");
  }
  if(jarak > 25){
    client.publish("ultrasonic1", "0");
  }
}

void capacityServo2(){
  digitalWrite(capacity2_Trig_Ultrasonic, LOW); 
  delayMicroseconds(2); 
  digitalWrite(capacity2_Trig_Ultrasonic, HIGH); 
  delayMicroseconds(10); 
  int duration = pulseIn(capacity2_Echo_Ultrasonic, HIGH); 
  int jarak = duration*0.034/2;

  if(jarak < 2){
    client.publish("ultrasonic2", "100");
  }
  if(jarak > 2 && jarak < 4){
    client.publish("ultrasonic2", "75");
  }
  if(jarak > 4 && jarak < 6){
    client.publish("ultrasonic2", "50");
  }
  if(jarak > 6 && jarak < 8){
    client.publish("ultrasonic2", "25");
  }
  if(jarak > 8 && jarak < 10){
    client.publish("ultrasonic2", "10");
  }
  if(jarak > 12){
    client.publish("ultrasonic2", "0");
  }
}

void setup(){
  pinMode(buzzer, OUTPUT);
  pinMode(capacity1_Echo_Ultrasonic, INPUT); 
  pinMode(capacity1_Trig_Ultrasonic, OUTPUT);
  pinMode(capacity2_Echo_Ultrasonic, INPUT); 
  pinMode(capacity2_Trig_Ultrasonic, OUTPUT);
  lcd.init();
  lcd.backlight();
  servo.attach(servo1);
  servo2.attach(D4);
  servo.write(CLOSE_ANGLE);
  servo2.write(CLOSE_ANGLE_2);  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop(){
  displaylcd();
  capacityServo1();
  capacityServo2();
   
  if(!client.connected()){
    reconnect();
  }
  client.loop();
}
