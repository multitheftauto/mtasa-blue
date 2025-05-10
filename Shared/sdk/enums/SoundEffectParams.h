/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/SoundEffectParams.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

namespace eSoundEffectParams
{
    enum class Chorus
    {
        WET_DRY_MIX,
        DEPTH,
        FEEDBACK,
        FREQUENCY,
        WAVEFORM,
        DELAY,
        PHASE,
    };

    enum class Compressor
    {
        GAIN,
        ATTACK,
        RELEASE,
        THRESHOLD,
        RATIO,
        PREDELAY,
    };

    enum class Distortion
    {
        GAIN,
        EDGE,
        POST_EQ_CENTER_FREQUENCY,
        POST_EQ_BANDWIDTH,
        PRE_LOWPASS_CUTOFF,
    };

    enum class Echo
    {
        WET_DRY_MIX,
        FEEDBACK,
        LEFT_DELAY,
        RIGHT_DELAY,
        PAN_DELAY,
    };

    enum class Flanger
    {
        WET_DRY_MIX,
        DEPTH,
        FEEDBACK,
        FREQUENCY,
        WAVEFORM,
        DELAY,
        PHASE,
    };

    enum class Gargle
    {
        RATE_HZ,
        WAVE_SHAPE,
    };

    enum class I3DL2Reverb
    {
        ROOM,
        ROOM_HF,
        ROOM_ROLLOFF_FACTOR,
        DECAY_TIME,
        DECAY_HF_RATIO,
        REFLECTIONS,
        REFLECTIONS_DELAY,
        REVERB,
        REVERB_DELAY,
        DIFFUSION,
        DENSITY,
        HF_REFERENCE,
    };

    enum class ParamEq
    {
        CENTER,
        BANDWIDTH,
        GAIN,
    };

    enum class Reverb
    {
        IN_GAIN,
        REVERB_MIX,
        REVERB_TIME,
        HIGH_FREQ_RT_RATIO,
    };
}            // namespace eSoundEffectParams
