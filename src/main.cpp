#pragma GCC optimize("O3") // code optimisation controls - "O2" & "O3" code performance, "Os" code size

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AiEsp32RotaryEncoder.h>

#define COLUMS 16             // LCD columns
#define ROWS 2                // LCD rows
#define LCD_SPACE_SYMBOL 0x20 // space symbol from LCD ROM, see p.9 of GDM2004D datasheet

#define ROTARY_ENCODER_A_PIN 34
#define ROTARY_ENCODER_B_PIN 35
#define ROTARY_ENCODER_BUTTON_PIN 32
#define ROTARY_ENCODER_VCC_PIN -1

#define ROTARY_ENCODER_STEPS 2 // depending on your encoder - try 1,2 or 4 to get expected behaviour

// instead of changing here, rather change numbers above
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

// paramaters for button
unsigned long shortPressAfterMiliseconds = 50;  // how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned long longPressAfterMiliseconds = 1000; // how long čong press shoud be.

volatile int shrtprsd = 0;
volatile int lngprsd = 0;
volatile int post = 0;

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void on_button_short_click()
{
  shrtprsd++;
}

void on_button_long_click()
{
  lngprsd++;
}

void handle_rotary_button()
{
  static unsigned long lastTimeButtonDown = 0;
  static bool wasButtonDown = false;

  bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
  // isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed

  if (isEncoderButtonDown)
  {
    Serial.print("+"); // REMOVE THIS LINE IF YOU DONT WANT TO SEE
    if (!wasButtonDown)
    {
      // start measuring
      lastTimeButtonDown = millis();
    }
    // else we wait since button is still down
    wasButtonDown = true;
    return;
  }

  // button is up
  if (wasButtonDown)
  {
    Serial.println(""); // REMOVE THIS LINE IF YOU DONT WANT TO SEE
    // click happened, lets see if it was short click, long click or just too short
    if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds)
    {
      on_button_long_click();
    }
    else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds)
    {
      on_button_short_click();
    }
  }
  wasButtonDown = false;
}
//********** button handling ----
//********** button handling ----
//********** button handling ----
//********** button handling ----

void rotary_loop()
{
  // dont print anything unless value changed
  if (rotaryEncoder.encoderChanged())
  {
    post = rotaryEncoder.readEncoder();
  }
  handle_rotary_button();
}

void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}

void setup()
{

  Serial.begin(115200);

  while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS) != 1) // colums, rows, characters size
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);
  }

  lcd.print(F("PCF8574 is OK...")); // F ) ) saves string to flash & keeps dynamic memory free
  delay(2000);

  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  // set boundaries and if values should cycle or not
  // in this example we will set possible values between 0 and 1000;
  bool circleValues = false;
  rotaryEncoder.setBoundaries(0, 1000, circleValues); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)

  /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
  // rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
  rotaryEncoder.setAcceleration(250); // or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration

  lcd.clear();
}

void loop()
{
  rotary_loop();
  lcd.setCursor(0, 1); // set 1-st column, 2-nd row
  lcd.print(F("BUTTON  :"));
  lcd.setCursor(10, 1);
  lcd.print(shrtprsd);
  lcd.setCursor(12, 1);
  lcd.print(lngprsd);
  lcd.setCursor(0, 0);
  lcd.print(F("POSITION:"));
  lcd.setCursor(10, 0);
  lcd.print("    ");
  lcd.setCursor(10, 0);
  lcd.print(post);

  /* Cree este codigo con bastantisima ayuda de los codes de ejemplo de las librerias, esto es una beta superrapida para poder montar el prototipo, aun no estoy familiarizado con freeRTOS ni las librerias, por eso esta tan mal optimizado el codigo, pero... funciona eh!*/