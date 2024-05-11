#include "TimerBaseCounter.hpp"

#include "../Logger.hpp"
#include "../Emulator.hpp"
#include "../Chipset/Chipset.hpp"

namespace casioemu
{
    void TimerBaseCounter::Initialise() {
        clock_type = CLOCK_LSCLK;

        LTBRCounter = 0;
        current_output = 0;
        LTBR_reset_tick = false;
    }

    void TimerBaseCounter::Reset() {
        LTBRCounter = 0;
        current_output = 0;
        LTBR_reset_tick = false;
    }

    void TimerBaseCounter::Tick() {
        if(LTBR_reset_tick) {
            LTBR_reset_tick = false;
            return;
        }

        emulator.chipset.LSCLK_output = 0;

        if(++LTBRCounter >= LTBROutputCount) {
            LTBRCounter = 0;
            emulator.chipset.data_LTBR++;
            current_output = emulator.chipset.LSCLK_output = (emulator.chipset.data_LTBR - 1) & (~emulator.chipset.data_LTBR);
            
            if(current_output & 0x01)
                emulator.chipset.MaskableInterrupts[L256SINT].TryRaise();
            if(current_output & 0x04)
                emulator.chipset.MaskableInterrupts[L1024SINT].TryRaise();
            if(current_output & 0x10)
                emulator.chipset.MaskableInterrupts[L4096SINT].TryRaise();
            if(current_output & 0x40)
                emulator.chipset.MaskableInterrupts[L16384SINT].TryRaise();
        }
    }

    void TimerBaseCounter::ResetLSCLK() {
        LTBRCounter = 0;
        emulator.chipset.LSCLK_output = 0xFF;
        LTBR_reset_tick = true;

        emulator.chipset.MaskableInterrupts[L256SINT].TryRaise();
        emulator.chipset.MaskableInterrupts[L1024SINT].TryRaise();
        emulator.chipset.MaskableInterrupts[L4096SINT].TryRaise();
        emulator.chipset.MaskableInterrupts[L16384SINT].TryRaise();
    }
}