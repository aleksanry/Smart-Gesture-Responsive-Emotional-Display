#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Tone.h>

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

// DeepSeek API 
const char* deepseekApiKey = "";
const char* deepseekApiUrl = "";

// Ultrasonic sensor pins
#define TRIG_PIN 5
#define ECHO_PIN 18

// Speaker pin
#define SPEAKER_PIN 25

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Initialize OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Variables for gesture detection
unsigned long gestureStartTime = 0;
bool isGestureActive = false;
float gestureDistance = 0;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Initialize OLED display
  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();

  // Initialize ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize speaker
  toneBegin(SPEAKER_PIN);

  // Display initial neutral face
  displayNeutralFace();
}

void loop() {
  // Measure distance from ultrasonic sensor
  float distance = measureDistance();

  // Check if a gesture is detected (hand within 10 cm)
  if (distance > 0 && distance < 10) {
    if (!isGestureActive) {
      // Record the start time of the gesture
      gestureStartTime = millis();
      gestureDistance = distance;
      isGestureActive = true;
    }
  } else {
    if (isGestureActive) {
      // Gesture ended
      isGestureActive = false;

      // Calculate gesture duration
      unsigned long gestureDuration = millis() - gestureStartTime;

      // Send gesture data to DeepSeek API
      String emotion = analyzeGesture(gestureDuration, gestureDistance);

      // Update display and speaker based on the emotion
      updateDisplayAndSpeaker(emotion);
    }
  }
}

// Function to measure distance using ultrasonic sensor
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2; // Convert to cm

  return distance;
}

// Function to send gesture data to DeepSeek API and get emotion
String analyzeGesture(unsigned long duration, float distance) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(deepseekApiUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + deepseekApiKey);

    // Create JSON payload
    String payload = "{\"duration\":" + String(duration) + ",\"distance\":" + String(distance) + "}";

    // Send POST request
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode == 200) {
      String response = http.getString();
      http.end();

      // Parse the response (assuming the API returns a JSON with an "emotion" field)
      int emotionStart = response.indexOf("\"emotion\":\"") + 11;
      int emotionEnd = response.indexOf("\"", emotionStart);
      String emotion = response.substring(emotionStart, emotionEnd);

      return emotion;
    } else {
      Serial.println("Error calling DeepSeek API");
      http.end();
      return "neutral";
    }
  } else {
    Serial.println("Wi-Fi not connected");
    return "neutral";
  }
}

// Function to update display and speaker based on emotion
void updateDisplayAndSpeaker(String emotion) {
  if (emotion == "happy") {
    displayHappyFace();
    playHappySound();
  } else if (emotion == "sad") {
    displaySadFace();
    playSadSound();
  } else if (emotion == "surprised") {
    displaySurprisedFace();
    playSurprisedSound();
  } else if (emotion == "angry") {
    displayAngryFace();
    playAngrySound();
  } else if (emotion == "sleepy") {
    displaySleepyFace();
    playSleepySound();
  } else {
    displayNeutralFace();
    noTone(SPEAKER_PIN);
  }
}

// Function to display a neutral face
void displayNeutralFace() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("-_-");
  display.display();
}

// Function to display a happy face
void displayHappyFace() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("^_^");
  display.display();
}

// Function to display a sad face
void displaySadFace() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("T_T");
  display.display();
}

// Function to display a surprised face
void displaySurprisedFace() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("O_O");
  display.display();
}

// Function to display an angry face
void displayAngryFace() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println(">_<");
  display.display();
}

// Function to display a sleepy face
void displaySleepyFace() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("~_~");
  display.display();
}

// Function to play a happy sound
void playHappySound() {
  tone(SPEAKER_PIN, 523, 200); // C5
  delay(200);
  tone(SPEAKER_PIN, 659, 200); // E5
  delay(200);
  tone(SPEAKER_PIN, 784, 200); // G5
  delay(200);
}

// Function to play a sad sound
void playSadSound() {
  tone(SPEAKER_PIN, 392, 300); // G4
  delay(300);
  tone(SPEAKER_PIN, 349, 300); // F4
  delay(300);
  tone(SPEAKER_PIN, 330, 300); // E4
  delay(300);
}

// Function to play a surprised sound
void playSurprisedSound() {
  tone(SPEAKER_PIN, 880, 300); // A5
  delay(300);
  tone(SPEAKER_PIN, 1047, 300); // C6
  delay(300);
}

// Function to play an angry sound
void playAngrySound() {
  tone(SPEAKER_PIN, 220, 200); // A3
  delay(200);
  tone(SPEAKER_PIN, 196, 200); // G3
  delay(200);
  tone(SPEAKER_PIN, 165, 200); // E3
  delay(200);
}

// Function to play a sleepy sound
void playSleepySound() {
  tone(SPEAKER_PIN, 262, 500); // C4
  delay(500);
  tone(SPEAKER_PIN, 294, 500); // D4
  delay(500);
  tone(SPEAKER_PIN, 330, 500); // E4
  delay(500);
}