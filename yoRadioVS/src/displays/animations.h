#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <Arduino.h>

// Animation frame structure for 20x2 LCD
struct AnimFrame {
	const char* line1;
	const char* line2;
};

// Define animation sequences
const AnimFrame fishAnimation[] = {
  {">'>            ", "        >'>     "},
  {" >'>           ", "          >'>   "},
  {"  >'>          ", "            >'> "},
  {"   >'>         ", "           >'>  "},
  {"    >'>        ", "         >'>    "},
  {"     >'>       ", "       >'>      "},
  {"      >'>      ", "     >'>        "},
  {"       >'>     ", "   >'>          "},
  {"        >'>    ", " >'>            "},
  {"         >'>   ", ">'>             "},
  {"          >'>  ", "'>              "},
  {"           >'> ", ">               "},
  {"            >'>", "                "},
  {"           >'> ", "                "},
  {"          >'>  ", "                "},
  {"         >'>   ", ">               "},
};

const AnimFrame starAnimation[] = {
  {"*    .    *    .   ", ".   *    .    *    "},
  {" *    .    *    .  ", "  .   *    .    *  "},
  {"  *    .    *    . ", "   .   *    .    * "},
  {"   *    .    *    .", "    .   *    .    *"},
};

const AnimFrame waveAnimation[] = {
  {"~~~~~~~~~~~~~~~~~~~~", "                    "},
  {" ~~~~~~~~~~~~~~~~~~~~", "~                   "},
  {"  ~~~~~~~~~~~~~~~~~~~", "~~                  "},
  {"   ~~~~~~~~~~~~~~~~~~", "~~~                 "},
  {"    ~~~~~~~~~~~~~~~~~", "~~~~                "},
  {"     ~~~~~~~~~~~~~~~~", "~~~~~               "},
};

// Animation configuration
enum AnimationType {
	ANIM_FISH,
	ANIM_STARS,
	ANIM_WAVES,
	ANIM_CLOCK_ONLY  // Keep existing clock behavior
};

struct Animation {
	const AnimFrame* frames;
	uint8_t frameCount;
	uint16_t frameDuration;  // milliseconds per frame
	AnimationType type;
};

// Animation definitions
const Animation animations[] = {
  {fishAnimation, sizeof(fishAnimation) / sizeof(AnimFrame), 150, ANIM_FISH},
  {starAnimation, sizeof(starAnimation) / sizeof(AnimFrame), 300, ANIM_STARS},
  {waveAnimation, sizeof(waveAnimation) / sizeof(AnimFrame), 200, ANIM_WAVES},
  {nullptr, 0, 0, ANIM_CLOCK_ONLY}
};

#endif