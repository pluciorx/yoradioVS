#include "../core/options.h"
#if DSP_MODEL==DSP_1602I2C || DSP_MODEL==DSP_1602 || DSP_MODEL==DSP_2004 || DSP_MODEL==DSP_2004I2C || DSP_MODEL==DSP_2002 || DSP_MODEL==DSP_2002I2C || DSP_MODEL==DSP_4002I2C
#include "dspcore.h"
#include <WiFi.h>
#include "../core/config.h"
#include "../core/network.h"
#include "../core/player.h"
#include "animations.h"

#if L10N_LANGUAGE == PL
#include "tools/polishChars.h"
#endif

#ifndef SCREEN_ADDRESS
  #define SCREEN_ADDRESS 0x27 ///< See datasheet for Address or scan it https://create.arduino.cc/projecthub/abdularbi17/how-to-scan-i2c-address-in-arduino-eaadda
#endif

// Static instance of animation controller
static LCDAnimationController lcdAnimController;

// LCDAnimationController implementation
LCDAnimationController::LCDAnimationController() {
  _currentFrame = 0;
  _totalFrames = 0;
  _lastUpdate = 0;
  _frameDuration = 0;
  _currentAnimation = ANIM_FISH;
  _animData = nullptr;
}

void LCDAnimationController::begin(AnimationType type) {
  _currentAnimation = type;
  _currentFrame = 0;
  _lastUpdate = millis();
  
  // Select appropriate animation based on display width
  #if DSP_MODEL==DSP_4002I2C
    // 40x2 display
    for(const auto& anim : animations40) {
      if(anim.type == type) {
        _animData = &anim;
        _totalFrames = anim.frameCount;
        _frameDuration = anim.frameDuration;
        break;
      }
    }
  #else
    // 20x2 or 16x2 displays
    for(const auto& anim : animations) {
      if(anim.type == type) {
        _animData = &anim;
        _totalFrames = anim.frameCount;
        _frameDuration = anim.frameDuration;
        break;
      }
    }
  #endif
}

bool LCDAnimationController::needsUpdate() {
  return (millis() - _lastUpdate >= _frameDuration);
}

void LCDAnimationController::update() {
  if(needsUpdate() && _animData != nullptr) {
    _currentFrame = (_currentFrame + 1) % _totalFrames;
    _lastUpdate = millis();
  }
}

const AnimFrame* LCDAnimationController::getCurrentFrame() {
  if(_animData == nullptr || _animData->frames == nullptr) return nullptr;
  return &(_animData->frames[_currentFrame]);
}

DspCore::DspCore(): DSP_INIT {
  _soundMeterMode = false;
  _soundMeterLastUpdate = 0;
  _soundMeterMeasL = 0;
  _soundMeterMeasR = 0;
}

void DspCore::apScreen() {
  clear();
  setCursor(0,0);
  print(utf8Rus(const_lcdApMode, false));
  setCursor(0,1);
  print(config.ipToStr(WiFi.softAPIP()));
#ifdef LCD_2004
  setCursor(0, 2);
  print(utf8Rus(const_lcdApName, false));
  print(apSsid);
  setCursor(0, 3);
  print(utf8Rus(const_lcdApPass, false));
  print(apPassword);
#endif
}

void DspCore::initDisplay() {
#ifdef LCD_I2C
  init();
  backlight();
#else
  #ifdef LCD_2004
    begin(20, 4);
  #elif DSP_MODEL==DSP_2002 || DSP_MODEL==DSP_2002I2C
    begin(20, 2);
  #else
    begin(16, 2);
  #endif
#endif

#if L10N_LANGUAGE == PL
  // Load Polish custom characters into CGRAM
  createChar(0, (uint8_t*)char_a_ogonek);  // ą
  createChar(1, (uint8_t*)char_c_acute);   // ć
  createChar(2, (uint8_t*)char_e_ogonek);  // ę
  createChar(3, (uint8_t*)char_l_stroke);  // ł
  createChar(4, (uint8_t*)char_n_acute);   // ń
  createChar(5, (uint8_t*)char_o_acute);   // ó
  createChar(6, (uint8_t*)char_s_acute);   // ś
  createChar(7, (uint8_t*)char_z_dot);     // ż
#endif

  clearClipping();
}

void DspCore::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
  if(w<2) return;
  char buf[width()+1] = { 0 };
  snprintf(buf, sizeof(buf), "%*s%s", w-1, "", " ");
  setCursor(x, y);
  print(buf);
  setCursor(x, y);
}

uint16_t DspCore::width(){
#if defined(LCD_2004) || DSP_MODEL==DSP_2002 || DSP_MODEL==DSP_2002I2C
  return 20;
#elif DSP_MODEL==DSP_4002I2C
	return 40;
#else
  return 16;
#endif
}

uint16_t DspCore::height(){
#ifdef LCD_2004
  return 4;
#else
  return 2;
#endif
}

void DspCore::clearDsp(bool black){ clear(); }
void DspCore::flip(){ }
void DspCore::invert(){ }
void DspCore::sleep(void) { 
  noDisplay();
#ifdef LCD_I2C
  noBacklight();
#endif
}
void DspCore::wake(void) { 
  display();
#ifdef LCD_I2C
  backlight();
#endif
}

// Animation methods for screensaver
void DspCore::showAnimationFrame(const AnimFrame* frame) {
    if (frame == nullptr) return;

    static char prevLine1[41] = "";
    static char prevLine2[41] = "";
    
    char line1[41];
    char line2[41];
    strcpy(line1, frame->line1);
    strcpy(line2, frame->line2);
    
    // Replace time placeholder
    char timeBuf[6];
    strftime(timeBuf, sizeof(timeBuf), "%H:%M", &network.timeinfo);
    char* pos = strstr(line1, "HH:MM");
    if (pos) {
        memcpy(pos, timeBuf, 5);
    }
    pos = strstr(line2, "HH:MM");
    if (pos) {
        memcpy(pos, timeBuf, 5);
    }

    // Replace date placeholder
    char dateBuf[11];
    strftime(dateBuf, sizeof(dateBuf), "%d/%m/%Y", &network.timeinfo);
    pos = strstr(line1, "DD/MM/YYYY");
    if (pos) {
        memcpy(pos, dateBuf, 10);
    }
    pos = strstr(line2, "DD/MM/YYYY");
    if (pos) {
        memcpy(pos, dateBuf, 10);
    }

    // Pad lines to full display width to clear old content
    uint16_t displayWidth = width();
    int len1 = strlen(line1);
    int len2 = strlen(line2);
    
    // Pad with spaces to full width
    for(int i = len1; i < displayWidth; i++) {
        line1[i] = ' ';
    }
    line1[displayWidth] = '\0';
    
    for(int i = len2; i < displayWidth; i++) {
        line2[i] = ' ';
    }
    line2[displayWidth] = '\0';

    // Only update lines that have changed
    if (strcmp(line1, prevLine1) != 0) {
        setCursor(0, 0);
        print(line1);
        strcpy(prevLine1, line1);
        yield();  // Let ESP32 service watchdog and background tasks
    }
    
    if (strcmp(line2, prevLine2) != 0) {
        setCursor(0, 1);
        print(line2);
        strcpy(prevLine2, line2);
        yield();  // Let ESP32 service watchdog and background tasks
    }
}

void DspCore::initScreensaver(AnimationType type) {
    lcdAnimController.begin(type);
    
    if (type == ANIM_SOUND_METER) {
        // Initialize sound meter mode
        _soundMeterMode = true;
        _soundMeterLastUpdate = 0;
        _soundMeterMeasL = 0;
        _soundMeterMeasR = 0;
        
        // Show clock on line 1
        showSoundMeterClock();
        // Clear line 2 for sound meter
        setCursor(0, 1);
        #if defined(LCD_4002)
          print("                                        "); // 40 spaces
        #elif defined(LCD_2004) || defined(LCD_2002)
          print("                    "); // 20 spaces
        #else
          print("                "); // 16 spaces
        #endif
    } else {
        _soundMeterMode = false;
        // Show first frame immediately
        const AnimFrame* frame = lcdAnimController.getCurrentFrame();
        showAnimationFrame(frame);
    }
}

void DspCore::updateScreensaver() {
    if (_soundMeterMode) {
        // Update sound meter
        if (millis() - _soundMeterLastUpdate >= 50) { // Update every 50ms
            _soundMeterLastUpdate = millis();
            updateSoundMeter();
            showSoundMeterClock(); // Update clock periodically
        }
    } else {
        // Regular animation
        if (lcdAnimController.needsUpdate()) {
            lcdAnimController.update();
            const AnimFrame* frame = lcdAnimController.getCurrentFrame();
            showAnimationFrame(frame);
        }
    }
}

void DspCore::showSoundMeterClock() {
    // Show time on line 1
    char timeBuf[6]; // HH:MM + null terminator
    strftime(timeBuf, sizeof(timeBuf), "%H:%M", &network.timeinfo);
    
    #if defined(LCD_4002)
      // Center on 40 char display
      char line[41];
      const int padding = (40 - strlen(timeBuf)) / 2;
      memset(line, ' ', 40);
      memcpy(line + padding, timeBuf, strlen(timeBuf));
      line[40] = '\0';
      setCursor(0, 0);
      print(line);
    #elif defined(LCD_2004) || defined(LCD_2002)
      // Center on 20 char display
      char line[21];
      const int padding = (20 - strlen(timeBuf)) / 2;
      memset(line, ' ', 20);
      memcpy(line + padding, timeBuf, strlen(timeBuf));
      line[20] = '\0';
      setCursor(0, 0);
      print(line);
    #else
      // Center on 16 char display
      char line[17];
      const int padding = (16 - strlen(timeBuf)) / 2;
      memset(line, ' ', 16);
      memcpy(line + padding, timeBuf, strlen(timeBuf));
      line[16] = '\0';
      setCursor(0, 0);
      print(line);
    #endif
}

void DspCore::updateSoundMeter() {
    // Get display width
    #if defined(LCD_4002)
      const uint8_t displayWidth = 40;
      const uint8_t halfWidth = 20;
    #elif defined(LCD_2004) || defined(LCD_2002)
      const uint8_t displayWidth = 20;
      const uint8_t halfWidth = 10;
    #else
      const uint8_t displayWidth = 16;
      const uint8_t halfWidth = 8;
    #endif
    
    // Get audio levels
    uint16_t vulevel = player.get_VUlevel(halfWidth);
    uint8_t L = (vulevel >> 8) & 0xFF;
    uint8_t R = vulevel & 0xFF;
    
    // Smooth fade
    const uint8_t fadeRate = 2;
    bool played = player.isRunning();
    
    if(played) {
        _soundMeterMeasL = (L >= _soundMeterMeasL) ? L : (_soundMeterMeasL > fadeRate ? _soundMeterMeasL - fadeRate : 0);
        _soundMeterMeasR = (R >= _soundMeterMeasR) ? R : (_soundMeterMeasR > fadeRate ? _soundMeterMeasR - fadeRate : 0);
    } else {
        if(_soundMeterMeasL > 0) _soundMeterMeasL = (_soundMeterMeasL > fadeRate) ? _soundMeterMeasL - fadeRate : 0;
        if(_soundMeterMeasR > 0) _soundMeterMeasR = (_soundMeterMeasR > fadeRate) ? _soundMeterMeasR - fadeRate : 0;
    }
    
    if(_soundMeterMeasL > halfWidth) _soundMeterMeasL = halfWidth;
    if(_soundMeterMeasR > halfWidth) _soundMeterMeasR = halfWidth;
    
    // Build sound meter line based on display width
    // Left channel: from left edge (0) towards center
    // Right channel: from right edge towards center
    #if defined(LCD_4002)
      char line[41]; // 40 chars + null
    #elif defined(LCD_2004) || defined(LCD_2002)
      char line[21]; // 20 chars + null
    #else
      char line[17]; // 16 chars + null
    #endif
    
    memset(line, ' ', displayWidth);
    
    // Fill left channel (from left)
    for(uint8_t i = 0; i < _soundMeterMeasL; i++) {
        line[i] = (char)0xFF; // Solid block character
    }
    
    // Fill right channel (from right)
    for(uint8_t i = 0; i < _soundMeterMeasR; i++) {
        line[displayWidth - 1 - i] = (char)0xFF; // Solid block character
    }
    
    line[displayWidth] = '\0';
    
    // Display on line 2
    setCursor(0, 1);
    print(line);
}

#endif
