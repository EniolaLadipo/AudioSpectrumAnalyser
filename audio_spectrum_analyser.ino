// empty strings require user's own entries
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""

#include <arduinoFFT.h>
#include <FastLED.h>
#include <BlynkSimpleEsp32.h>

// Blynk WiFi credentials
char ssid[] = "";
char pass[] = "";
char auth[] = "";

#define LED_PIN 2 // sends data from ESP32 to LED matrix
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define ANALOG_PIN 34 // receives data from analog microphone

// I set to 50 to avoid excessive power consumption (but feel free to experiment with higher settings)
#define MAX_BRIGHTNESS 50

// FFT setup
const uint16_t samples = 64; // number of FFT samples
const double samplingFrequency = 10000; // adjust sampling frequency as needed
double vReal[samples];
double vImag[samples];
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, samplingFrequency);

// LED setup
CRGB leds[NUM_LEDS];

void displaySpectrum(double* spectrum, uint16_t size);

bool isPowerOn = false;

// toggle button to turn device on and off, set to Virtual Pin 0
BLYNK_WRITE(V0) {
  isPowerOn = param.asInt();
}

void setup() {
  Serial.begin(115200);
  
  // Blynk setup
  Blynk.begin(auth, ssid, pass);
  
  // LED setup
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear(true);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  
  Serial.println("Audio Spectrum Analyzer initialized!");
}

void loop() {

  if (isPowerOn) {

    // capturing audio samples
    double dcOffset = 0.0;
    for (uint16_t i = 0; i < samples; i++) {
      vReal[i] = analogRead(ANALOG_PIN);
      vImag[i] = 0.0;
      dcOffset += vReal[i];
      delayMicroseconds(1000000 / samplingFrequency);
    }

    // removing DC offset (without this, lower bands will remain high even without sound, so the leftside of the matrix will remain lit)
    dcOffset /= samples;
    for (uint16_t i = 0; i < samples; i++) {
      vReal[i] -= dcOffset;
    }

    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);

    FFT.compute(FFTDirection::Forward);

    FFT.complexToMagnitude();

    // convert signal energy (RMS) to decibel (dB) to displat to Blynk
    double rms = 0; // RMS - Root Mean Square
    for (uint16_t i = 0; i < samples; i++) {
      rms += vReal[i] * vReal[i];
    }

    // decibel calculation
    rms = sqrt(rms / samples);
    double reference = 15; // reference value for decibels (adjust for your mic)
    double dB = 20 * log10(rms / reference);

    Blynk.virtualWrite(V2, dB); // decibel readings sent to Blynk via Virtual Pin V2
    
    displaySpectrum(vReal, samples / 2);
  }

  Blynk.run();

  delay(30);
}

// map (x, y) coordinates to LED index for a vertical serpentine matrix
uint16_t getLEDIndex(uint8_t x, uint8_t y) {
  if (x % 2 == 0) {
    // even columns: top to bottom
    return x * MATRIX_HEIGHT + y;
  } else {
    // odd columns: bottom to top
    return x * MATRIX_HEIGHT + (MATRIX_HEIGHT - 1 - y);
  }
}

// map frequency bins to the LED matrix
void displaySpectrum(double* spectrum, uint16_t size) {

  FastLED.clear(); // clear LED matrix for the next frame

  uint8_t bands = MATRIX_WIDTH;
  uint8_t ledsPerBand = MATRIX_HEIGHT;

  for (uint8_t band = 0; band < bands; band++) {
    // map the FFT output to an appropriate band
    uint16_t startBin = (band * (size / bands));
    uint16_t endBin = ((band + 1) * (size / bands)) - 1;

    // average the magnitudes for the current band
    double magnitude = 0;
    for (uint16_t bin = startBin; bin <= endBin; bin++) {
      magnitude += spectrum[bin];
    }
    magnitude /= (endBin - startBin + 1);

    // map the magnitude to the number of LEDs lit in this band
    uint8_t numLEDs = map(magnitude, 0, 2000, 0, ledsPerBand); // adjust max value (2000) as needed, this will affect how high the columns can light up in response to sound
    numLEDs = constrain(numLEDs, 0, ledsPerBand);

    // light up LEDs in the column for this band
    for (uint8_t y = 0; y < numLEDs; y++) {
      uint16_t index = getLEDIndex(band, ledsPerBand - 1 - y); // map (x, y) to LED index
      leds[index] = CHSV(band * 8, 255, 255); // colours bands from left to right in rainbow (low pitch = red, high pitch = violet)
    }
  }

  FastLED.show();
}
