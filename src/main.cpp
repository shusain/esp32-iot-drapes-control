#include <Arduino.h>

#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <CheapStepper.h>

// WifiConfig.h
// File should be edited to include your local wifi credentials so the ESP32
// can connect to your network (as well as IP of MQTT broker/server)
#include <WifiConfig.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;

WiFiClient espClient;
PubSubClient client(espClient);

// Keeps track of when the last message was sent
long lastMsg = 0;

// Used to track state of drapes
boolean isOpen = true;
// Used to track state of motor in terms of number of rotations (0 totally open MAX_ROTATIONS when totally closed)
int currentRotation = 0;
const int MAX_ROTATIONS = 8;

// Cheap stepper setup
CheapStepper cs = CheapStepper(25,26,27,14);
CheapStepper cs2 = CheapStepper(15,2,4,17);

void  setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WifiConfig::ssid);

  WiFi.begin(WifiConfig::ssid, WifiConfig::password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  // Debug messages via serial log
  // Serial.print("Message arrived on topic: ");
  // Serial.print(topic);
  // Serial.print(". Message: ");
  // Serial.println(messageTemp);

  if (String(topic) == "bedroom/drapes/set-state") {
    if(messageTemp == "open") {
      isOpen = true;
    }
    if(messageTemp == "closed") {
      isOpen = false;
    }
  } else if (String(topic) == "bedroom/drapes") {
    if((messageTemp == "open" || messageTemp == "open-complete") && currentRotation>0){
	    cs.setDirection(1);
	    cs2.setDirection(-1);

      int stepsToTake = stepsPerRevolution;
      if(messageTemp == "open-complete") {
        stepsToTake = stepsPerRevolution * currentRotation;
        isOpen = true;
        currentRotation=0;
      } else {
        currentRotation--;
        if(currentRotation==0) {
          isOpen = true;
        }
      }

      for (size_t i = 0; i < stepsToTake; i++)
      {
        cs.move_stepper(1);
        cs2.move_stepper(1);
      }
      cs.stop();
      cs2.stop();
    }
    else if((messageTemp == "close" || messageTemp == "close-complete") && currentRotation<MAX_ROTATIONS){
	    cs.setDirection(-1);
	    cs2.setDirection(1);
      int stepsToTake = stepsPerRevolution;
      if(messageTemp == "close-complete") {
        isOpen = false;
        stepsToTake = stepsPerRevolution * (MAX_ROTATIONS-currentRotation);
        currentRotation = MAX_ROTATIONS;
      } else {
        currentRotation++;
        if(currentRotation == MAX_ROTATIONS) {
          isOpen = false;
        }
      }
      for (size_t i = 0; i < stepsToTake; i++)
      {
        cs.move_stepper(1);
        cs2.move_stepper(1);
      }
      cs.stop();
      cs2.stop();
    }
  }
}

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  setup_wifi();
  client.setServer(WifiConfig::mqttServer, 1883);
  client.setCallback(callback);
	ESP32PWM::allocateTimer(0);
  cs.setup();
  cs2.setup();
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("drapes-module", "drapes", "test")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("bedroom/drapes");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  long now = millis();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    if(isOpen) {
      client.publish("bedroom/drapes/open", "yes they are");
    } else {
      client.publish("bedroom/drapes/openState", "no they aren't");
    }
    client.publish("bedroom/drapes/onfire", "they are not");
  }
}