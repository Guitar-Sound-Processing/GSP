package com.example.gsp.storage

import android.content.Context
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken
import kotlin.collections.toMutableList
import androidx.core.content.edit

class PresetManager(private val context: Context) {

    // Effect preset
    private val prefs =
        context.getSharedPreferences("gsp_presets", Context.MODE_PRIVATE)

    private var chainId = 1L
    private val gsonCompact = Gson()

    private fun formatPresetsListJson(presets: List<EffectPreset>): String {
        if (presets.isEmpty()) return "[]"
        val lines = presets.joinToString(",\n") { "\t" + gsonCompact.toJson(it) }
        return "[\n$lines\n]"
    }

    private fun formatChainsJson(chains: List<ChainPreset>): String {
        if (chains.isEmpty()) return "[]"
        val lines = chains.joinToString(",\n") { "\t" + gsonCompact.toJson(it) }
        return "[\n$lines\n]"
    }

    //************************************************************************
    // Effect Presets
    fun savePreset(effect: String, preset: EffectPreset) {
        val presets = loadPresets(effect).toMutableList()
        presets.removeAll { it.name == preset.name }
        presets.add(preset)
        val json = formatPresetsListJson(presets)
        prefs.edit { putString(effect, json) }
    }

    fun loadPresets(effect: String): List<EffectPreset> {
        var json = prefs.getString(effect, null)
        if (json == null) {
            StorageInitializer(context).initializeDefaultDataIfNeeded()
            json = prefs.getString(effect, null)
        }
        if (json == null) return emptyList()
        val type = object : TypeToken<List<EffectPreset>>() {}.type
        return gsonCompact.fromJson(json, type)
    }

    fun deletePreset(effect: String, name: String) {
        val presets = loadPresets(effect).toMutableList()
        presets.removeAll { it.name == name }
        val json = formatPresetsListJson(presets)
        prefs.edit { putString(effect, json) }
    }

    //************************************************************************
    // Chain preset

    fun saveChain(preset: ChainPreset) {
        val chains = loadChains().toMutableList()
        val index = chains.indexOfFirst { it.id == preset.id }
        if (index >= 0)
            chains[index] = preset
        else
            chains.add(preset)
        val json = formatChainsJson(chains)
        prefs.edit { putString("chains", json) }
    }

    fun loadChains(): List<ChainPreset> {
        var json = prefs.getString("chains", null)
        if (json == null) {
            StorageInitializer(context).initializeDefaultDataIfNeeded()
            json = prefs.getString("chains", null)
        }
        if (json == null) return emptyList()
        val type = object : TypeToken<List<ChainPreset>>() {}.type
        return gsonCompact.fromJson(json, type)
    }

    fun deleteChain(id: Long) {
        val chains = loadChains().toMutableList()
        chains.removeAll { it.id == id }
        val json = formatChainsJson(chains)
        prefs.edit { putString("chains", json) }
    }

    fun findChain(name: String): ChainPreset? {
        return loadChains().firstOrNull { it.name == name }
    }
}
