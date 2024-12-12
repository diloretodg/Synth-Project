#include <Arduino.h>  
#include <TFT_eSPI.h>  
#include <driver/i2s.h>  

#define SAMPLE_RATE 44100  
#define BUFFER_SIZE 128  
#define I2S_PORT I2S_NUM_0  
#define I2S_COMM_FORMAT_STAND_I2S (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB)  

TFT_eSPI tft = TFT_eSPI();  
float phase = 0.0f;  
float frequency = 440.0f;  

void setup() {  
  // Initialize Serial with proper baud rate and wait for it to be ready  
  Serial.begin(115200);  
  delay(1000);  // Give Serial time to initialize  
  Serial.println("\nCYD Synth Starting...");  

  // Initialize display  
  Serial.println("Initializing display...");  
  tft.init();  
  tft.setRotation(3);  
  tft.fillScreen(TFT_BLACK);  
  tft.setTouch(true);  // Enable touch

  // Draw test pattern to verify display  
  Serial.println("Drawing test pattern...");  
  tft.drawRect(0, 0, WAVE_WIDTH, WAVE_HEIGHT, TFT_RED);  
  tft.drawLine(0, 0, WAVE_WIDTH, WAVE_HEIGHT, TFT_BLUE);  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);  
  tft.drawString("CYD Synth Test", 10, 10, 4);  
  delay(2000);  

  // Configure I2S  
  Serial.println("Configuring I2S...");  
  i2s_config_t i2s_config = {  
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),  
    .sample_rate = SAMPLE_RATE,  
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,  
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,  
    .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_I2S,  
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,  
    .dma_buf_count = 8,  
    .dma_buf_len = BUFFER_SIZE,  
    .use_apll = false,  
    .tx_desc_auto_clear = true,  
    .fixed_mclk = 0  
  };  

  i2s_pin_config_t pin_config = {  
    .bck_io_num = 25,  
    .ws_io_num = 26,  
    .data_out_num = 27,  
    .data_in_num = I2S_PIN_NO_CHANGE  
  };  

  esp_err_t result = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);  
  if (result != ESP_OK) {  
    Serial.println("Error installing I2S driver");  
    while(1) {  
      delay(1000);  
      Serial.println("I2S installation failed");  
    }  
  }  
  Serial.println("I2S driver installed successfully");  

  result = i2s_set_pin(I2S_PORT, &pin_config);  
  if (result != ESP_OK) {  
    Serial.println("Error setting I2S pins");  
    while(1) {  
      delay(1000);  
      Serial.println("I2S pin configuration failed");  
    }  
  }  
  Serial.println("I2S pins configured successfully");  
  Serial.println("Setup complete!");  
}  

void loop() {  
  static int16_t samples[BUFFER_SIZE * 2];  
  static uint16_t waveform[WAVE_WIDTH];  
  static unsigned long lastDebugPrint = 0;  

  // Generate samples  
  for(int i = 0; i < BUFFER_SIZE; i++) {  
    float sample = sin(2 * PI * phase);  
    int16_t intSample = sample * 32767;  
    samples[i*2] = intSample;  
    samples[i*2+1] = intSample;  

    phase += frequency / SAMPLE_RATE;  
    if(phase >= 1.0f) phase -= 1.0f;  

    if(i < WAVE_WIDTH) {  
      // Map to middle 50% of screen height for better visibility  
      waveform[i] = map(intSample, -32767, 32767, WAVE_HEIGHT/4, (WAVE_HEIGHT*3)/4);  
    }  
  }  

  // Output audio  
  size_t bytes_written;  
  i2s_write(I2S_PORT, samples, BUFFER_SIZE * 4, &bytes_written, portMAX_DELAY);  

  // Debug print every second  
  if (millis() - lastDebugPrint > 1000) {  
    Serial.printf("Frequency: %.2f Hz, Phase: %.4f\n", frequency, phase);  
    Serial.printf("Waveform values[0-4]: %d, %d, %d, %d, %d\n",   
                 waveform[0], waveform[1], waveform[2], waveform[3], waveform[4]);  
    lastDebugPrint = millis();  
  }  

  // Update display  
  tft.fillScreen(TFT_BLACK);  

  // Draw center reference line  
  tft.drawFastHLine(0, WAVE_HEIGHT/2, WAVE_WIDTH, TFT_DARKGREY);  

  // Draw waveform  
  for(int i = 0; i < WAVE_WIDTH-1; i++) {  
    tft.drawLine(i, waveform[i], i+1, waveform[i+1], TFT_GREEN);  
  }  

  // Display frequency  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);  
  tft.drawString("Freq: " + String(frequency) + " Hz", 10, 10, 2);  

  // Handle touch input  
  uint16_t x, y;  
  if (tft.getTouch(&x, &y)) {  
    frequency = map(x, 0, WAVE_WIDTH, 100, 2000);  
    Serial.printf("Touch detected: x=%d, y=%d, new freq=%.2f\n", x, y, frequency);  
  }  
}  
