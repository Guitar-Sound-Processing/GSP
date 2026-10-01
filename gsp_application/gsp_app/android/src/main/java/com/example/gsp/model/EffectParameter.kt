package com.example.gsp.model

open class EffectParameter(
    //val id: String,
    val name: String,
    val min: Float,
    val max: Float,
    val low: Float,
    val high: Float,
    val unit: String,
    val value: Float,
    val type: ParamType,
    val lfo: LfoParameter? = null
) {
    enum class ParamType {
        FLOAT,
        INT,
        LFO
    }
    fun toMappedValue(): Float {
        return min + value * (max - min)
    }

    fun fromMappedValue(mapped: Float): Float {
        return (mapped - min) / (max - min)
    }

    fun toSliderValue(): Float {
        return low + value * (high - low)
    }
    fun copy(
        name: String = this.name,
        min: Float = this.min,
        max: Float = this.max,
        low: Float = this.low,
        high: Float = this.high,
        unit: String = this.unit,
        value: Float = this.value,
        type: ParamType = this.type,
        lfo: LfoParameter? = this.lfo
    ): EffectParameter {
        return EffectParameter(name, min, max, low, high, unit, value, type, lfo)
    }
}
