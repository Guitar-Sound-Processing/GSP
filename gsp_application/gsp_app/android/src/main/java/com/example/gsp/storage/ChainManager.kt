package com.example.gsp.storage

import android.content.Context

/**
 * Gerencia as Chains salvas pelo aplicativo.
 *
 * Responsabilidades:
 * - listar Chains salvas
 * - localizar uma Chain pelo nome ou ID
 * - salvar uma Chain
 * - excluir uma Chain
 * - gerar novos IDs para Chains
 *
 * A persistência propriamente dita continua sendo feita pelo PresetManager.
 */
class ChainManager(context: Context) {

    private val presetManager = PresetManager(context)

    /**
     * Retorna todas as Chains salvas.
     */
    fun getChains(): List<ChainPreset> {
        return presetManager.loadChains()
    }

    /**
      * Localiza uma Chain pelo nome.
     */
    fun findByName(name: String): ChainPreset? {
        return presetManager.findChain(name)
    }

    /**
     * Localiza uma Chain pelo ID.
     */
    fun findById(id: Long): ChainPreset? {
        return presetManager
            .loadChains()
            .firstOrNull { it.id == id }
    }
    /**
     * Salva uma Chain.
     *
     * Se já existir uma Chain com o mesmo nome, o ID existente
     * é preservado e a Chain é atualizada.
     *
     * Se for uma Chain nova, recebe um novo ID.
     */
    fun saveChain(
        name: String,
        commands: List<String>
    ): ChainPreset {

        val existing = findByName(name)

        val id = existing?.id ?: nextId()

        val preset = ChainPreset(
            id = id,
            name = name,
            commands = commands.toMutableList()
        )

        presetManager.saveChain(preset)

        return preset
    }

    /**
     * Exclui uma Chain pelo nome.
     *
     * Retorna true se uma Chain foi encontrada e excluída.
     */
    fun deleteByName(name: String): Boolean {

        val existing = findByName(name)
            ?: return false

        presetManager.deleteChain(existing.id)

        return true
    }

    /**
     * Exclui uma Chain pelo ID.
     *
     * Retorna true se uma Chain foi encontrada e excluída.
     */
    fun deleteById(id: Long): Boolean {

        val existing = findById(id)
            ?: return false

        presetManager.deleteChain(existing.id)

        return true
    }

    /**
     * Gera o próximo ID disponível.
     *
     * O maior ID existente é procurado e incrementado em 1.
     * Assim, não dependemos de um contador mantido em memória.
     */
    private fun nextId(): Long {

        val chains = getChains()

        val highestId =
            chains.maxOfOrNull { it.id } ?: 0L

        return highestId + 1L
    }
}