#if defined(IRADIO_TOUCH_DIAGNOSTIC)
#include "Display.h"
#include <Wire.h>

namespace {
LGFX lcd;
constexpr uint8_t touchAddress = 0x38;
uint32_t lastReport = 0;
bool wasTouched = false;
int previousX = -1, previousY = -1;

// Read TD_STATUS and the first contact directly, independent of IRQ and LGFX mapping.
bool readContact(uint8_t& count, uint16_t& x, uint16_t& y) {
  Wire.beginTransmission(touchAddress);
  Wire.write(0x02);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(touchAddress, static_cast<uint8_t>(5)) != 5) return false;
  count = Wire.read() & 0x0f;
  const uint8_t xHigh = Wire.read();
  const uint8_t xLow = Wire.read();
  const uint8_t yHigh = Wire.read();
  const uint8_t yLow = Wire.read();
  x = ((xHigh & 0x0f) << 8) | xLow;
  y = ((yHigh & 0x0f) << 8) | yLow;
  return true;
}
} // namespace

void setup() {
  Serial.begin(115200);
  const uint32_t start = millis();
  while (!Serial && millis() - start < 1500) delay(10);
  pinMode(FN_AUDIO_ENABLE, OUTPUT);
  digitalWrite(FN_AUDIO_ENABLE, HIGH); // Keep the amplifier muted during diagnosis.
  pinMode(FN_TOUCH_INT, INPUT_PULLUP);
  const bool busReady = Wire.begin(FN_TOUCH_SDA, FN_TOUCH_SCL, 400000);
  lcd.init();
  delay(200); // Allow the touch controller to settle after its reset pulse.
  lcd.setRotation(1);
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.setCursor(8, 8);
  lcd.println("FNK0104B touch diagnostic");
  lcd.println("Touch corners and center; watch serial.");
  lcd.drawCircle(16, 48, 8, TFT_WHITE);
  lcd.drawCircle(lcd.width() - 17, 48, 8, TFT_WHITE);
  lcd.drawCircle(16, lcd.height() - 17, 8, TFT_WHITE);
  lcd.drawCircle(lcd.width() - 17, lcd.height() - 17, 8, TFT_WHITE);
  lcd.drawCircle(lcd.width() / 2, lcd.height() / 2, 8, TFT_WHITE);
  Wire.beginTransmission(touchAddress);
  const uint8_t status = Wire.endTransmission();
  Serial.printf("Touch test: SDA=%d SCL=%d RESET=%d INT=%d polling=%d\n",
                FN_TOUCH_SDA, FN_TOUCH_SCL, FN_TOUCH_RST, FN_TOUCH_INT,
                !FN_TOUCH_USE_IRQ);
  Serial.printf("I2C begin=%d; controller 0x38 status=%u (0 means ACK)\n", busReady, status);
}

void loop() {
  uint8_t count = 0;
  uint16_t rawX = 0, rawY = 0;
  const bool readOK = readContact(count, rawX, rawY);
  int x = 0, y = 0;
  const bool mapped = lcd.getTouch(&x, &y);
  const bool touched = readOK && count != 0;
  const uint32_t now = millis();
  if ((touched || mapped) ? now - lastReport >= 100
                         : (wasTouched || now - lastReport >= 2000)) {
    Serial.printf("read=%d contacts=%u raw=(%u,%u) INT=%d mapped=%d xy=(%d,%d)\n",
                  readOK, count, rawX, rawY, digitalRead(FN_TOUCH_INT), mapped, x, y);
    lastReport = now;
  }
  wasTouched = touched || mapped;
  if (mapped && x >= 0 && x < lcd.width() && y >= 0 && y < lcd.height()) {
    if (previousX >= 0) lcd.fillCircle(previousX, previousY, 3, TFT_BLACK);
    lcd.fillCircle(x, y, 3, TFT_GREEN);
    previousX = x;
    previousY = y;
  }
  delay(20);
}
#endif
