package com.example.gsp.model

fun getAllEffectIds(): List<String> {
    return loadInitialEffects().map { it.id }
}

fun getAllEffects(): List<Effect> {
    return loadInitialEffects()
}

fun loadInitialEffects(): List<Effect> {
    return listOf(

        //->LVD: Attack (0.2-)(ms): 1.000 | Release (0.2-)(ms): 1000.000
        Effect(
            id = "lvd",
            name = "Level Detector",
            change = false,
            position = -1,
            parameters = listOf(
                EffectParameter( name = "Attack",
                    min = 0.2f, max = 2000f, low = 0.2f, high = 2000f, unit = "ms", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Release",
                    min = 0.2f, max = 2000f, low = 0.2f, high = 2000f, unit = "ms", value = 1000f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        //->CHS (8): OFF(0)|ON(1) 0 | Depth (0.1-100)(ms): 5.0 | Delay (0-1000)(ms): 1.0 | Mixer: 0.500 |
        // Profile: (0-10) 0 | Frequency (0.2-5)(Hz): 0.500 | Duty Cycle (0-100)(): 50.0 | Gain (0-1): 1.000
        Effect(
            id = "chs",
            name = "Chorus",
            change = true,
            position = 0,
            hasLfo = true,
            parameters = listOf(
                EffectParameter( name = "Depth",
                    min = 0.1f, max = 100f, low = 0.1f, high = 100f, unit = "ms", value = 5.0f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Delay",
                    min = 0f, max = 1000f, low = 0f, high = 1000f, unit = "ms", value = 1.0f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Mixer",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 0.5f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Profile",
                    min = 0f, max = 1f, low = 0f, high = 1f, unit = "", value = 0f,
                    type = EffectParameter.ParamType.LFO,
                    lfo = LfoParameter(
                        waveform = 0,
                        frequency = EffectParameter( name = "Frequency",
                            min = 0.2f, max = 5f, low = 0.2f, high = 5f, unit = "Hz", value = 0.5f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        dutyCycle = EffectParameter( name = "Duty Cycle",
                            min = 0f, max = 100f, low = 0f, high = 100f, unit = "%", value = 50f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        pedalChannel = 0
                    )
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1.0f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->CMP (0): OFF(0)|ON(1) 0 | Attack (20-2000)(ms): 10.0 | Release (20-2000)(ms): 1000.0 |
        // Gain (0-80)(dB): 20 | Threshold (0-80)(dB): 40
        Effect(
            id = "cmp",
            name = "Compressor",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Attack",
                    min = 0.2f, max = 2000f, low = 0.2f, high = 2000f, unit = "ms", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Release",
                    min = 0.2f, max = 2000f, low = 0.2f, high = 2000f, unit = "ms", value = 1000f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 80f, low = 0f, high = 80f, unit = "dB", value = 20.0f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Threshold",
                    min = 0f, max = 80f, low = 0f, high = 80f, unit = "dB", value = 40.0f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->DFB (11): OFF(0)|ON(1) 0 | Delay Time (0.2-100)(ms): 31.0 | Decay rate (0-0.95): 0.700
        // | Gain (0-1): 1.000
        Effect(
            id = "dfb",
            name = "Delay Feedback",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Delay",
                    min = 0.2f, max = 100f, low = 0.2f, high = 100f, unit = "ms", value = 31f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Decay rate",
                    min = 0f, max = 0.95f, low = 0f, high = 0.95f, unit = "", value = 0.7f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1.0f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        //->DFF (13): OFF(0)|ON(1) 0 | Delay Time (0.2-100)(ms): 31.0 | Decay rate (0-1): 0.900
        // | Number of repeats (1-8): 4 | Gain (0-1): 1.000
        Effect(
            id = "dff",
            name = "Delay Feedforward",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Delay",
                    min = 0.2f, max = 100f, low = 0.2f, high = 100f, unit = "ms", value = 31f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Decay rate",
                    min = 0f, max = 1f, low = 0f, high = 1f, unit = " ", value = 0.9f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Repeats",
                    min = 1f, max = 8f, low = 1f, high = 8f, unit = " ", value = 4f,
                    type = EffectParameter.ParamType.INT
                ),
                EffectParameter( name = "Vol",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->DTN (5): OFF(0)|ON(1) 0 | Detune (down) (0-12): 5.000 | Mixer (0-1): 0.500 |
        // Gain (0-1): 1.000
        Effect(
            id = "dtn",
            name = "Detune",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Detune pitch",
                    min = 0f, max = 12f, low = 0f, high = 12f, unit = "ST", value = 5f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Mixer",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 0.5f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->EFB (12): OFF(0)|ON(1) 0 | Delay Time (50-)(ms): 1000.0 | Decay rate (0-0.95): 0.700
        // | Gain (0-1): 1.000
        Effect(
            id = "efb",
            name = "Echo Feedback",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Delay",
                    min = 50f, max = 2000f, low = 50f, high = 2000f, unit = "ms", value = 1000f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Decay rate",
                    min = 0f, max = 0.95f, low = 0f, high = 0.95f, unit = " ", value = 0.7f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->EFF (14): OFF(0)|ON(1) 0 | Delay Time (50-)(ms): 1000.0 | Decay rate (0-1): 0.900
        // | Number of repeats (1-8): 4 | Gain (0-1): 1.000
        Effect(
            id = "eff",
            name = "Echo Feedforward",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Delay",
                    min = 50f, max = 2000f, low = 50f, high = 2000f, unit = "ms", value = 1000f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Decay rate",
                    min = 0f, max = 1f, low = 0f, high = 1f, unit = " ", value = 0.9f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Repeats",
                    min = 1f, max = 8f, low = 1f, high = 8f, unit = " ", value = 4f,
                    type = EffectParameter.ParamType.INT
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->EQZ (7): OFF(0)|ON(1) 0 | Gains (0-1): Low 1.000 , Medium 1.000 , High 1.000
        // | Cutoff frequencies (Hz): Low (200-) 200.000 , High (-2000) 800.0
        Effect(
            id = "eqz",
            name = "Equalizer",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Low Freq Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Medium Freq Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "High Freq Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Low Cutoff Freq",
                    min = 100f, max = 2000f, low = 100f, high = 2000f, unit = "Hz", value = 200f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "High Cutoff Freq",
                    min = 100f, max = 2000f, low = 100f, high = 2000f, unit = "Hz", value = 2000f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->LIM (17): OFF(0)|ON(1) 0 | Smooth factor (0-1): 1.000 | Gain (0- ): 1.000
        Effect(
            id = "lim",
            name = "Limiter",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Smooth factor",
                    min = 0f, max = 1f, low = 0f, high = 1f, unit = " ", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Input Gain",
                    min = 0.1f, max = 2f, low = 0.1f, high = 2f, unit = "", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->NGT (18): OFF(0)|ON(1) 0 | Attack (20-2000)(ms): 10.0 | Release (20-2000)(ms): 1000.0
        // | Gain (0.1-1): 1.000 | Threshold (0-1): 0.100
        Effect(
            id = "ngt",
            name = "Noise Gate",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Attack",
                    min = 20f, max = 2000f, low = 20f, high = 2000f, unit = "ms", value = 10f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Release",
                    min = 20f, max = 2000f, low = 20f, high = 2000f, unit = "ms", value = 1000f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Gain",
                    min = 0.1f, max = 1f, low = 10f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Threshold",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 10f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // >OCT (3): OFF(0)|ON(1) 0 | Mixer (0-1): 0.500 | Gain (0-1): 1.000
        Effect(
            id = "oct",
            name = "Octave",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Mixer",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 0.5f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->OVD (1): OFF(0)|ON(1) 0 | Sustain (0.1-1): 0.500 | Tone (0-1): 0.800 |
        // Mixer (0-1): 1.000 | Gain (0-1): 1.000
        Effect(
            id = "ovd",
            name = "Overdrive",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Sustain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Tone",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Mixer",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 0.5f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Vol",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->PHR (2): OFF(0)|ON(1) 0 | Depth (0-1): 0.500 | Level (0-1000): 10.0 |
        // Profile: (0-10) 0 | Frequency (0.2-5)(Hz): 0.250 | Duty Cycle (0-100)(): 50.0
        // | Gain (0-1): 1.000
        Effect(
            id = "phr",
            name = "Phaser",
            change = true,
            position = 0,
            hasLfo = true,
            parameters = listOf(
                EffectParameter( name = "Depth",
                    min = 0f, max = 1f, low = 0f, high = 1f, unit = " ", value = 0.5f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Level",
                    min = 0f, max = 1000f, low = 0f, high = 1000f, unit = " ", value = 10f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Profile",
                    min = 0f, max = 1f, low = 0f, high = 1f, unit = "", value = 0f,
                    type = EffectParameter.ParamType.LFO,
                    lfo = LfoParameter(
                        waveform = 0,
                        frequency = EffectParameter( name = "Frequency",
                            min = 0.2f, max = 5f, low = 0.2f, high = 5f, unit = "Hz", value = 0.25f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        dutyCycle = EffectParameter( name = "Duty Cycle",
                            min = 0f, max = 100f, low = 0f, high = 100f, unit = "%", value = 50f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        pedalChannel = 0
                    )
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1.0f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->SFT (4): OFF(0)|ON(1) 0 | Shift (up) (0-12): 5.000 | Mixer (0-1): 0.500 |
        // Gain (0-1): 1.000
        Effect(
            id = "sft",
            name = "Pitch Shifter",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Shift tone",
                    min = 0f, max = 12f, low = 0f, high = 12f, unit = "ST", value = 5f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Mixer",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 0.5f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->RVB (10): OFF(0)|ON(1) 0 | Reverber Time (0-20000)(ms): 1000.0 | Gain (0-1): 1.000
        Effect(
            id = "rvb",
            name = "Reverb",
            change = true,
            position = 0,
            hasLfo = false,
            parameters = listOf(
                EffectParameter( name = "Reverber Time",
                    min = 0f, max = 2000f, low = 0f, high = 2000f, unit = "ms", value = 1000f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // TML (15): OFF(0)|ON(1) 0 | Profile: (0-10) 1 | Frequency (0.2-5)(Hz): 2.000 |
        // Duty Cycle (0-100)(): 50.0 | Gain (0-1): 1.000
        Effect(
            id = "tml",
            name = "Tremolo",
            change = true,
            position = 0,
            hasLfo = true,
            parameters = listOf(
                EffectParameter(
                    name = "Profile",
                    min = 0f, max = 1f, low = 0f, high = 1f, unit = "", value = 0f,
                    type = EffectParameter.ParamType.LFO,
                    lfo = LfoParameter(
                        waveform = 1,
                        frequency = EffectParameter( name = "Freq",
                            min = 0.2f, max = 5f, low = 0.2f, high = 5f, unit = "Hz", value = 0.5f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        dutyCycle = EffectParameter( name = "Duty Cycle",
                            min = 0f, max = 100f, low = 0f, high = 100f, unit = "%", value = 0.5f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        pedalChannel = 0
                    )
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->VBT (9): OFF(0)|ON(1) 0 | Depth (0.1-100)(ms): 5.0 | Delay (0-1000)(ms): 1.0 |
        // Profile: (0-10) 0 | Frequency (0.2-5)(Hz): 0.500 | Duty Cycle (0-100)(): 50.0 |
        // Gain (0-1): 1.000
        Effect(
            id = "vbt",
            name = "Vibrato",
            change = true,
            position = 0,
            hasLfo = true,
            parameters = listOf(
                EffectParameter( name = "Depth",
                    min = 0.1f, max = 100f, low = 0.1f, high = 100f, unit = "ms", value = 5.0f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Delay",
                    min = 0f, max = 1000f, low = 0f, high = 1000f, unit = "ms", value = 1.0f,
                    type = EffectParameter.ParamType.FLOAT
                ),
                EffectParameter( name = "Profile",
                    min = 0f, max = 1f, low = 0f, high = 1f, unit = "", value = 0f,
                    type = EffectParameter.ParamType.LFO,
                    lfo = LfoParameter(
                        waveform = 0,
                        frequency = EffectParameter( name = "Frequency",
                            min = 0.2f, max = 5f, low = 0.2f, high = 5f, unit = "Hz", value = 0.5f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        dutyCycle = EffectParameter( name = "Duty Cycle",
                            min = 0f, max = 100f, low = 0f, high = 100f, unit = "%", value = 50f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        pedalChannel = 0
                    )
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1.0f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->VOL (16): OFF(0)|ON(1) 0 | Profile: (0-10) 1 | Frequency (0.2-5)(Hz): 2.000 |
        // Duty Cycle (0-100)(): 50.0 | Gain (0-1): 1.000
        Effect(
            id = "vol",
            name = "Volume",
            change = true,
            position = 0,
            hasLfo = true,
            parameters = listOf(
                EffectParameter( name = "Profile",
                    min = 0f, max = 1f, low = 0f, high = 1f, unit = "", value = 0f,
                    type = EffectParameter.ParamType.LFO,
                    lfo = LfoParameter(
                        waveform = 1,
                        frequency = EffectParameter( name = "Frequency",
                            min = 0.2f, max = 5f, low = 0.2f, high = 5f, unit = "Hz", value = 2f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        dutyCycle = EffectParameter( name = "Duty Cycle",
                            min = 0f, max = 100f, low = 0f, high = 100f, unit = "%", value = 50f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        pedalChannel = 0
                    )
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1.0f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        ),

        // ->WAH (6): OFF(0)|ON(1) 0 | Profile: (0-10) 1 | Frequency (0.2-5)(Hz): 2.000 |
        // Duty Cycle (0-100)(): 50.0 | Gain (0-1): 1.000
        Effect(
            id = "wah",
            name = "Wah Wah",
            change = true,
            position = 0,
            hasLfo = true,
            parameters = listOf(
                EffectParameter( name = "Profile",
                    min = 0f, max = 1f, low = 0f, high = 1f, unit = "", value = 0f,
                    type = EffectParameter.ParamType.LFO,
                    lfo = LfoParameter(
                        waveform = 1,
                        frequency = EffectParameter( name = "Frequency",
                            min = 0.2f, max = 5f, low = 0.2f, high = 5f, unit = "Hz", value = 2f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        dutyCycle = EffectParameter( name = "Duty Cycle",
                            min = 0f, max = 100f, low = 0f, high = 100f, unit = "%", value = 50f,
                            type = EffectParameter.ParamType.FLOAT
                        ),
                        pedalChannel = 0
                    )
                ),
                EffectParameter( name = "Gain",
                    min = 0f, max = 1f, low = 0f, high = 100f, unit = "%", value = 1.0f,
                    type = EffectParameter.ParamType.FLOAT
                )
            )
        )
    )
}