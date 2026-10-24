package com.example.gsp.viewmodel

import com.example.gsp.model.Effect
import com.example.gsp.model.EffectParameter
import java.util.Locale

object DaisyProtocolHandler {

    // Constantes do Protocolo de Comunicação
    const val PREFIX_INBOUND = "->"
    const val CMD_CLEAR = "clr"
    const val CMD_LVD = "lvd"
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
     * Decodifica mensagens de atualização de parâmetros de efeitos enviadas pelo Daisy Seed.
     */
    fun parseMessage(msg: String): Triple<String, Pair<Int, Boolean>, List<Float>>? {
        if (!msg.startsWith(PREFIX_INBOUND)) {
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
