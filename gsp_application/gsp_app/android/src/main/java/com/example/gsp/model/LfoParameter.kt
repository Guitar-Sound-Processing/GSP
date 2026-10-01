package com.example.gsp.model

data class LfoParameter(
    val waveform: Int,
    val frequency: EffectParameter,
    val dutyCycle: EffectParameter,
    val pedalChannel: Int = 0
) {

    companion object {
        val waveforms = listOf(
            "Sine",
            "Half Sine",
            "Ramp",
            "Saw",
            "Triangle",
            "Square",
            "Exp Descend",
            "Exp Ascent",
            "Express Pedal",
            "Power Level",
            "Reverse Level"
        )

        val hasFrequency = listOf(
            true,  // Sine
            true,  // Half Sine
            true,  // Ramp
            true,  // Saw
            true,  // Triangle
            true,  // Square
            true,  // Exp Desc
            true,  // Exp Asc
            false, // Expression Pedal 🔥
            false, // Power Level
            false  // Reverse Power Level
        )

        val hasDuty = listOf(
            false, // Sine
            false, // Half Sine
            false, // Ramp
            false, // Saw
            false, // Triangle
            true,  // Square 🔥
            true,  // Exp Desc
            true,  // Exp Asc
            false, // Expression Pedal 🔥
            false, // Power Level
            false  // Reverse Power Level
        )
    }

    fun waveformName(): String {
        return waveforms.getOrElse(waveform) { "Unknown" }
    }
}
