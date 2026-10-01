package com.example.gsp.viewmodel

import com.example.gsp.model.Effect
import com.example.gsp.model.EffectParameter
import com.example.gsp.model.LfoParameter
import java.util.Locale

object DaisyProtocolHandler {

    // Constantes do Protocolo de Comunicação
    const val PREFIX_POT = "->POT"
    const val PREFIX_INBOUND = "->"
    const val CMD_CLEAR = "clr"
    const val CMD_LVD = "lvd"
    const val CMD_POT = "pot"
    const val DEFAULT_CHAIN_NAME = "No Chain"

    /**
     * Serializa um efeito para uma string de comando para o Daisy Seed.
     */
    fun buildCommand(
        effect: Effect,
        write: Boolean
    ): String {
        if (!write) {
            return effect.id
        }
        val values = effect.parameters
            .flatMap { param ->
                when (param.type) {
                    EffectParameter.ParamType.FLOAT,
                    EffectParameter.ParamType.INT -> {
                        listOf(param.toMappedValue())
                    }
                    EffectParameter.ParamType.LFO -> {
                        val lfo = param.lfo ?: return@flatMap emptyList()
                        listOf(
                            lfo.waveform.toFloat(),
                            lfo.frequency.toMappedValue(),
                            lfo.dutyCycle.toMappedValue()
                        )
                    }
                }
            }
            .joinToString(" ") {
                "%.3f".format(Locale.US, it)
            }

        return if (effect.change) {
            val enabled = if (effect.enabled) 1 else 0
            "${effect.id} $enabled $values"
        } else {
            "${effect.id} $values"
        }
    }

    /**
     * Constrói o comando para o pedal de expressão (POT).
     */
    fun buildPotCommand(effect: Effect): String? {
        val lfo = effect.parameters
            .firstOrNull { it.lfo != null }
            ?.lfo ?: return null

        if (lfo.waveformName() != "Express Pedal" || lfo.pedalChannel < 0) {
            return null
        }
        return "$CMD_POT ${effect.id} ${lfo.pedalChannel}"
    }

    /**
     * Decodifica mensagens do tipo POT recebidas do Daisy Seed.
     */
    fun parsePot(msg: String): Map<String, Int>? {
        if (!msg.startsWith(PREFIX_POT)) {
            return null
        }
        val parts = msg
            .removePrefix(PREFIX_POT)
            .trim()
            .split(" ")
            .filter { it.isNotBlank() }

        if (parts.isEmpty()) {
            return emptyMap()
        }
        val result = mutableMapOf<String, Int>()
        var i = 0
        while (i + 1 < parts.size) {
            val id = parts[i].lowercase()
            val channel = parts[i + 1].toIntOrNull()
            if (channel != null) {
                result[id] = channel
            }
            i += 2
        }
        return result
    }

    /**
     * Decodifica mensagens de atualização de parâmetros de efeitos enviadas pelo Daisy Seed.
     */
    fun parseMessage(msg: String): Triple<String, Pair<Int, Boolean>, List<Float>>? {
        if (!msg.startsWith(PREFIX_INBOUND) || msg.startsWith(PREFIX_POT)) {
            return null
        }
        val clean = msg
            .removePrefix(PREFIX_INBOUND)
            .replace("(", "")
            .replace(")", "")
        val parts = clean
            .split(" ")
            .filter { it.isNotBlank() }

        if (parts.size < 3) {
            return null
        }
        val id = parts[0].lowercase()
        val position = (parts[1].toIntOrNull() ?: -1) + 1
        val enabled = parts[2].toIntOrNull() == 1
        val values = parts
            .drop(3)
            .mapNotNull { it.toFloatOrNull() }

        return Triple(id, Pair(position, enabled), values)
    }

    /**
     * Decodifica mensagens contendo o estado atual da cadeia (Chain) enviado pelo Daisy Seed.
     */
    fun parseChain(msg: String): List<String> {
        return msg
            .removePrefix(PREFIX_INBOUND)
            .trim()
            .split(Regex("\\s+"))
            .mapNotNull { token ->
                val clean = token
                    .replace("(", "")
                    .replace(")", "")
                    .trim()
                if (clean.isBlank()) null else clean.lowercase()
            }
    }
}
