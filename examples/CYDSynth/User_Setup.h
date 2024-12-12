#define USER_SETUP_INFO "User_Setup"  

// Driver  
#define ILI9341_DRIVER  

// Display pins  
#define TFT_MISO 12  // This is correct  
#define TFT_MOSI 13  // This is correct  
#define TFT_SCLK 14  // This is correct  
#define TFT_CS   15  // This is correct  
#define TFT_DC    2  // This is correct  
#define TFT_RST   4  // This needs to be -1 for CYD  

// Touch pins  
#define TOUCH_CS 33  // This should be 21 for CYD  

// Fonts  
#define LOAD_GLCD  
#define LOAD_FONT2  
#define LOAD_FONT4  
#define LOAD_FONT6  
#define LOAD_FONT7  
#define LOAD_FONT8  
#define LOAD_GFXFF  
#define SMOOTH_FONT  

// SPI Speeds  
#define SPI_FREQUENCY  40000000  
#define SPI_TOUCH_FREQUENCY  2500000  

// Touch screen calibration  
#define TOUCH_CALIBRATION_DATA { 209, 3563, 304, 3592, 4 }  
#define TOUCH_CALIBRATION  