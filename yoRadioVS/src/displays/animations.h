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

// Fish animation for 40x2
const AnimFrame fishAnimation40[] = {
  {">'>                                    ", "                                    >'>"},
  {" >'>                                   ", "                                   >'>"},
  {"  >'>                                  ", "                                  >'>"},
  {"   >'>                                 ", "                                 >'>"},
  {"    >'>                                ", "                                >'>"},
  {"     >'>                               ", "                               >'>"},
  {"      >'>                              ", "                              >'>"},
  {"       >'>                             ", "                             >'>"},
  {"        >'>                            ", "                            >'>"},
  {"         >'>                           ", "                           >'>"},
  {"          >'>                          ", "                          >'>"},
  {"           >'>                         ", "                         >'>"},
  {"            >'>                        ", "                        >'>"},
  {"             >'>                       ", "                       >'>"},
  {"              >'>                      ", "                      >'>"},
  {"               >'>                     ", "                     >'>"},
  {"                >'>                    ", "                    >'>"},
  {"                 >'>                   ", "                   >'>"},
  {"                  >'>                  ", "                  >'>"},
  {"                   >'>                 ", "                 >'>"},
  {"                    >'>                ", "                >'>"},
  {"                     >'>               ", "               >'>"},
  {"                      >'>              ", "              >'>"},
  {"                       >'>             ", "             >'>"},
  {"                        >'>            ", "            >'>"},
  {"                         >'>           ", "           >'>"},
  {"                          >'>          ", "          >'>"},
  {"                           >'>         ", "         >'>"},
  {"                            >'>        ", "        >'>"},
  {"                             >'>       ", "       >'>"},
  {"                              >'>      ", "      >'>"},
  {"                               >'>     ", "     >'>"},
  {"                                >'>    ", "    >'>"},
  {"                                 >'>   ", "   >'>"},
  {"                                  >'>  ", "  >'>"},
  {"                                   >'> ", " >'>"},
  {"                                    >'>", ">'>"}
};

// Star animation for 40x2
const AnimFrame starAnimation40[] = {
  {"*    .    *    .    *    .    *    .  ", ".    *    .    *    .    *    .    *   "},
  {" *    .    *    .    *    .    *    . ", "  .    *    .    *    .    *    .    * "},
  {"  *    .    *    .    *    .    *    .", "   .    *    .    *    .    *    .    *"},
  {"   *    .    *    .    *    .    *    ", "    .    *    .    *    .    *    .    "}
};

// Wave animation for 40x2
const AnimFrame waveAnimation40[] = {
  {"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", "                                        "},
  {" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", "~                                       "},
  {"  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", "~~                                      "},
  {"   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", "~~~                                     "},
  {"    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", "~~~~                                    "},
  {"     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", "~~~~~                                   "},
  {"      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", "~~~~~~                                  "},
  {"       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", "~~~~~~~                                 "},
  {"        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", "~~~~~~~~                                "},
  {"         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", "~~~~~~~~~                               "}
};

// Bouncing clock animation for 40x2 (positions for time display)
const AnimFrame clockAnimation40[] = {
  {"            HH:MM:SS                    ", "          DD/MM/YYYY                    "},
  {"               HH:MM:SS                 ", "             DD/MM/YYYY                 "},
  {"                  HH:MM:SS              ", "                DD/MM/YYYY              "},
  {"                     HH:MM:SS           ", "                   DD/MM/YYYY           "},
  {"                        HH:MM:SS        ", "                      DD/MM/YYYY        "},
  {"                     HH:MM:SS           ", "                   DD/MM/YYYY           "},
  {"                  HH:MM:SS              ", "                DD/MM/YYYY              "},
  {"               HH:MM:SS                 ", "             DD/MM/YYYY                 "}
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

// Animation definitions for 40x2 displays
const Animation animations40[] = {
  {fishAnimation40, sizeof(fishAnimation40) / sizeof(AnimFrame), 150, ANIM_FISH},
  {starAnimation40, sizeof(starAnimation40) / sizeof(AnimFrame), 300, ANIM_STARS},
  {waveAnimation40, sizeof(waveAnimation40) / sizeof(AnimFrame), 200, ANIM_WAVES},
  {clockAnimation40, sizeof(clockAnimation40) / sizeof(AnimFrame), 1000, ANIM_CLOCK_ONLY}
};

#endif