#include <Arduino.h>
#include <ezButton.h>

// =========================
// Pin Definitions
// =========================
#define VRX_PIN 34
#define VRY_PIN 35
#define SW_PIN  32

// =========================
// Direction Enum
// =========================
enum JoyDirection {
  CENTER,
  TOP,
  BOTTOM,
  LEFT,
  RIGHT,
  TOP_LEFT,
  TOP_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT
};

// =========================
// Joystick Class
// =========================
class Joystick {
  private:
    int vrxPin;
    int vryPin;
    ezButton button;

    int xValue = 0;
    int yValue = 0;

    // ESP32 ADC midpoint
    const int MID = 2048;

    // Deadzone size
    const int DEADZONE = 400;

  public:
    Joystick(int xPin, int yPin, int swPin)
      : vrxPin(xPin), vryPin(yPin), button(swPin) {}

    void begin() {
      analogSetAttenuation(ADC_11db);
      button.setDebounceTime(50);
    }

    void update() {
      button.loop();

      xValue = analogRead(vrxPin);
      yValue = analogRead(vryPin);
    }

    int getX() {
      return xValue;
    }

    int getY() {
      return yValue;
    }

    int getButtonState() {
      return button.getState();
    }

    bool isPressed() {
      return button.isPressed();
    }

    bool isReleased() {
      return button.isReleased();
    }

    JoyDirection getDirection() {

      // FIXED: Y-axis inverted
      bool up    = yValue < (MID - DEADZONE);
      bool down  = yValue > (MID + DEADZONE);

      bool left  = xValue < (MID - DEADZONE);
      bool right = xValue > (MID + DEADZONE);

      // Diagonal directions
      if (up && left) return TOP_LEFT;
      if (up && right) return TOP_RIGHT;
      if (down && left) return BOTTOM_LEFT;
      if (down && right) return BOTTOM_RIGHT;

      // Main directions
      if (up) return TOP;
      if (down) return BOTTOM;
      if (left) return LEFT;
      if (right) return RIGHT;

      return CENTER;
    }

    String directionToString(JoyDirection dir) {
      switch (dir) {
        case TOP: return "TOP";
        case BOTTOM: return "BOTTOM";
        case LEFT: return "LEFT";
        case RIGHT: return "RIGHT";
        case TOP_LEFT: return "TOP_LEFT";
        case TOP_RIGHT: return "TOP_RIGHT";
        case BOTTOM_LEFT: return "BOTTOM_LEFT";
        case BOTTOM_RIGHT: return "BOTTOM_RIGHT";
        default: return "CENTER";
      }
    }
};

// =========================
// Create Joystick Object
// =========================
Joystick joystick(VRX_PIN, VRY_PIN, SW_PIN);

// =========================
// Setup
// =========================
void setup() {
  Serial.begin(9600);

  joystick.begin();

  Serial.println("Joystick Ready");
}

// =========================
// Loop
// =========================
void loop() {
  joystick.update();

  JoyDirection dir = joystick.getDirection();

  Serial.print("X: ");
  Serial.print(joystick.getX());

  Serial.print(" | Y: ");
  Serial.print(joystick.getY());

  Serial.print(" | Direction: ");
  Serial.print(joystick.directionToString(dir));

  Serial.print(" | Button: ");
  Serial.println(joystick.getButtonState());

  // Button events
  if (joystick.isPressed()) {
    Serial.println("Button Pressed");
  }

  if (joystick.isReleased()) {
    Serial.println("Button Released");
  }

  delay(100);
}