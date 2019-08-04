
#include "gbsynth.hpp"

#include "tables.h"

// each channel has a maximum volume of 0.2, so maximum volume of all channels is 0.8
#define VOL_MULTIPLIER 0.2f

namespace gbsynth {

    Mixer::Mixer() :
        s01enable(DEFAULT_TERM_ENABLE),
        s02enable(DEFAULT_TERM_ENABLE),
        s01vol(DEFAULT_TERM_VOLUME),
        s02vol(DEFAULT_TERM_VOLUME),
        outputStat(OutputFlags::all_off)
    {
    }

    void Mixer::getOutput(float in1, float in2, float in3, float in4, float &outLeft, float &outRight) {
        float left = 0.0f, right = 0.0f;
        if (s01enable) {
            if ((outputStat & OutputFlags::left1) == OutputFlags::left1) {
                left += in1 * VOL_MULTIPLIER;
            }
            if ((outputStat & OutputFlags::left2) == OutputFlags::left2) {
                left += in2 * VOL_MULTIPLIER;
            }
            if ((outputStat & OutputFlags::left3) == OutputFlags::left3) {
                left += in3 * VOL_MULTIPLIER;
            }
            if ((outputStat & OutputFlags::left4) == OutputFlags::left4) {
                left += in4 * VOL_MULTIPLIER;
            }
        }
        if (s02enable) {
            if ((outputStat & OutputFlags::right1) == OutputFlags::right1) {
                right += in1 * VOL_MULTIPLIER;
            }
            if ((outputStat & OutputFlags::right2) == OutputFlags::right2) {
                right += in2 * VOL_MULTIPLIER;
            }
            if ((outputStat & OutputFlags::right3) == OutputFlags::right3) {
                right += in3 * VOL_MULTIPLIER;
            }
            if ((outputStat & OutputFlags::right4) == OutputFlags::right4) {
                right += in4 * VOL_MULTIPLIER;
            }
        }
        outLeft = left * VOLUME_TABLE[s01vol];
        outRight = right * VOLUME_TABLE[s02vol];
    }

    void Mixer::setTerminalEnable(Terminal term, bool enabled) {
        if (term == Terminal::s01) {
            s01enable = enabled;
        } else {
            s02enable = enabled;
        }
    }

    void Mixer::setTerminalVolume(Terminal term, uint8_t volume) {
        if (volume > MAX_VOLUME) {
            volume = MAX_VOLUME;
        }
        if (term == Terminal::s01) {
            s01vol = volume;
        } else {
            s02vol = volume;
        }
    }

    void Mixer::setEnable(OutputFlags flags) {
        outputStat = flags;
    }


}