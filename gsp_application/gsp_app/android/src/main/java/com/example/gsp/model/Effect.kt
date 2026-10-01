package com.example.gsp.model

data class Effect(
    val id: String,          // "ovd", "rvb"
    val name: String,        // "Overdrive"
    val enabled: Boolean = true,
    val change: Boolean,    // can change position (LVD)
    val position: Int,      // effect position in chain
    val hasLfo: Boolean = false,
    val parameters: List<EffectParameter>
)

