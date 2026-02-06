#include "../core/options.h"
#if DSP_MODEL==DSP_1602I2C || DSP_MODEL==DSP_1602 || DSP_MODEL==DSP_2004 || DSP_MODEL==DSP_2004I2C || DSP_MODEL==DSP_2002 || DSP_MODEL==DSP_2002I2C || DSP_MODEL==DSP_4002I2C
#include "dspcore.h"
#include <WiFi.h>
#include "../core/config.h"
#include "../core/network.h"
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

DspCore::DspCore(): DSP_INIT {}

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
    // Show first frame immediately
    const AnimFrame* frame = lcdAnimController.getCurrentFrame();
    showAnimationFrame(frame);
}

void DspCore::updateScreensaver() {
    if (lcdAnimController.needsUpdate()) {
        lcdAnimController.update();
        const AnimFrame* frame = lcdAnimController.getCurrentFrame();
        showAnimationFrame(frame);
    }
}

#endif
