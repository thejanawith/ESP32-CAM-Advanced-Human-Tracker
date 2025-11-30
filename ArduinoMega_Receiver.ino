/**
 * Arduino Mega Receiver for ESP32-CAM Human Tracker
 * 
 * Commands:
 * S - Stop
 * F - Forward  
 * L - Turn Left
 * R - Turn Right
 * 
 * Connections:
 * Pin 18 (TX1) -> ESP32-CAM GPIO14 (RX)
 * Pin 19 (RX1) -> ESP32-CAM GPIO15 (TX)
 * 
 * GitHub: https://github.com/yourusername/ESP32-CAM-Advanced-Human-Tracker
 */

String receivedCommand = "";
String lastCommand = "S";
unsigned long lastCommandTime = 0;

// Motor control pins - adjust according to your setup
const int leftMotorForward = 2;
const int leftMotorBackward = 3;
const int rightMotorForward = 4;
const int rightMotorBackward = 5;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);  // Hardware serial for ESP32-CAM
  
  // Initialize motor pins
  pinMode(leftMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);
  
  // Initialize status LED
  pinMode(13, OUTPUT);
  
  Serial.println("🤖 ARDUINO MEGA - TRACKING RECEIVER");
  Serial.println("===================================");
  Serial.println("Waiting for ESP32-CAM commands...");
  Serial.println("S = Stop, F = Forward, L = Left, R = Right");
  Serial.println("===================================\n");
  
  // Start in stopped state
  stopMotors();
}

void loop() {
  // Read commands from ESP32-CAM
  if (Serial1.available()) {
    char incomingChar = Serial1.read();
    
    if (incomingChar == '\n') {
      processTrackerCommand(receivedCommand);
      receivedCommand = "";
    } else {
      receivedCommand += incomingChar;
    }
    
    lastCommandTime = millis();
    digitalWrite(13, HIGH);  // Blink LED on command received
    delay(50);
    digitalWrite(13, LOW);
  }
  
  // Safety feature: stop if no commands received for 2 seconds
  if (millis() - lastCommandTime > 2000) {
    safetyStop();
  }
  
  delay(10);
}

/**
 * Process commands from ESP32-CAM tracker
 */
void processTrackerCommand(String command) {
  // Avoid processing the same command repeatedly
  if (command == lastCommand) {
    return;
  }
  
  Serial.print("📍 Command received: ");
  Serial.println(command);
  
  if (command == "S") {
    stopMotors();
    Serial.println("🛑 ACTION: STOP");
  } 
  else if (command == "F") {
    moveForward();
    Serial.println("🔼 ACTION: FORWARD");
  }
  else if (command == "L") {
    turnLeft();
    Serial.println("↩️ ACTION: TURN LEFT");
  }
  else if (command == "R") {
    turnRight();
    Serial.println("↪️ ACTION: TURN RIGHT");
  }
  else {
    Serial.println("❓ UNKNOWN COMMAND");
    return;
  }
  
  lastCommand = command;
}

/**
 * Emergency stop when communication is lost
 */
void safetyStop() {
  stopMotors();
  if (lastCommand != "S") {
    Serial.println("🚨 SAFETY: AUTO STOP - NO SIGNAL");
    lastCommand = "S";
  }
}

// ========================================
// MOTOR CONTROL FUNCTIONS
// ========================================

void stopMotors() {
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(rightMotorBackward, LOW);
}

void moveForward() {
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(rightMotorBackward, LOW);
}

void turnLeft() {
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(leftMotorBackward, HIGH);
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(rightMotorBackward, LOW);
}

void turnRight() {
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(rightMotorBackward, HIGH);
}
