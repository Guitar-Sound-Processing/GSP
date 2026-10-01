package com.example.gsp.storage

import android.content.Context
import androidx.core.content.edit
import com.example.gsp.model.Playlist
import com.example.gsp.model.Song
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken

/**
 * Responsável por carregar os dados JSON iniciais da pasta assets/
 * e copiá-los para o SharedPreferences na primeira inicialização do aplicativo.
 */
class StorageInitializer(private val context: Context) {

    private val gsonCompact = Gson()

    fun initializeDefaultDataIfNeeded() {
        val appPrefs = context.getSharedPreferences("gsp_app_settings", Context.MODE_PRIVATE)
        val isInitialized = appPrefs.getBoolean("initial_data_loaded", false)
        if (isInitialized) return

        copyInitialSongs()
        copyInitialPlaylists()
        copyInitialChains()
        copyInitialPresets()

        appPrefs.edit { putBoolean("initial_data_loaded", true) }
    }

    private fun loadAssetString(fileName: String): String? {
        return try {
            context.assets.open(fileName).bufferedReader().use { it.readText() }
        } catch (e: Exception) {
            e.printStackTrace()
            null
        }
    }

    private fun formatSongsJson(songs: List<Song>): String {
        if (songs.isEmpty()) return "[]"
        val lines = songs.joinToString(",\n") { "\t" + gsonCompact.toJson(it) }
        return "[\n$lines\n]"
    }

    private fun formatPlaylistsJson(playlists: List<Playlist>): String {
        if (playlists.isEmpty()) return "[]"
        val lines = playlists.joinToString(",\n") { "\t" + gsonCompact.toJson(it) }
        return "[\n$lines\n]"
    }

    private fun formatChainsJson(chains: List<ChainPreset>): String {
        if (chains.isEmpty()) return "[]"
        val lines = chains.joinToString(",\n") { "\t" + gsonCompact.toJson(it) }
        return "[\n$lines\n]"
    }

    private fun formatPresetsListJson(presets: List<EffectPreset>): String {
        if (presets.isEmpty()) return "[]"
        val lines = presets.joinToString(",\n") { "\t" + gsonCompact.toJson(it) }
        return "[\n$lines\n]"
    }

    private fun copyInitialSongs() {
        val songPrefs = context.getSharedPreferences("songs", Context.MODE_PRIVATE)
        if (songPrefs.contains("songs")) return

        val jsonString = loadAssetString("initial_songs.json") ?: return
        val type = object : TypeToken<List<Song>>() {}.type
        val songs: List<Song> = gsonCompact.fromJson(jsonString, type) ?: emptyList()

        songPrefs.edit {
            putString("songs", formatSongsJson(songs))
        }
    }

    private fun copyInitialPlaylists() {
        val playlistPrefs = context.getSharedPreferences("playlists", Context.MODE_PRIVATE)
        if (playlistPrefs.contains("playlists")) return

        val jsonString = loadAssetString("initial_playlists.json") ?: return
        val type = object : TypeToken<List<Playlist>>() {}.type
        val playlists: List<Playlist> = gsonCompact.fromJson(jsonString, type) ?: emptyList()

        playlistPrefs.edit {
            putString("playlists", formatPlaylistsJson(playlists))
        }
    }

    private fun copyInitialChains() {
        val presetPrefs = context.getSharedPreferences("gsp_presets", Context.MODE_PRIVATE)
        if (presetPrefs.contains("chains")) return

        val jsonString = loadAssetString("initial_chains.json") ?: return
        val type = object : TypeToken<List<ChainPreset>>() {}.type
        val chains: List<ChainPreset> = gsonCompact.fromJson(jsonString, type) ?: emptyList()

        presetPrefs.edit {
            putString("chains", formatChainsJson(chains))
        }
    }

    private fun copyInitialPresets() {
        val presetPrefs = context.getSharedPreferences("gsp_presets", Context.MODE_PRIVATE)

        val jsonString = loadAssetString("initial_presets.json") ?: return
        val type = object : TypeToken<Map<String, List<EffectPreset>>>() {}.type
        val presetsMap: Map<String, List<EffectPreset>> = gsonCompact.fromJson(jsonString, type) ?: emptyMap()

        presetPrefs.edit {
            for ((effectId, presetsList) in presetsMap) {
                if (!presetPrefs.contains(effectId)) {
                    putString(effectId, formatPresetsListJson(presetsList))
                }
            }
        }
    }
}
