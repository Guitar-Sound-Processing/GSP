package com.example.gsp.storage

import android.content.Context
import com.example.gsp.model.Playlist
import com.example.gsp.model.Song
import com.google.gson.GsonBuilder
import com.google.gson.reflect.TypeToken
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

enum class ImportMode {
    MERGE,      // Lossless Append
    OVERWRITE   // Overwrite / Replace All
}

data class BackupParseResult(
    val data: GspBackupData? = null,
    val errorMessage: String? = null
)

class BackupManager(context: Context) {

    private val songManager = SongManager(context)
    private val playlistManager = PlaylistManager(context)
    private val presetManager = PresetManager(context)
    private val gson = GsonBuilder().setPrettyPrinting().create()

    fun createBackupJson(): String {
        val songs = songManager.loadSongs()
        val playlists = playlistManager.loadPlaylists()
        val chains = presetManager.loadChains()

        val effectIds = listOf("phr", "ovd", "chs", "tml", "dly", "rev", "cmp", "eq", "flg", "wah", "ng", "pft", "lvd")
        val presetsMap = mutableMapOf<String, List<EffectPreset>>()
        for (id in effectIds) {
            val list = presetManager.loadPresets(id)
            if (list.isNotEmpty()) {
                presetsMap[id] = list
            }
        }

        val timestamp = SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.US).format(Date())
        val backupData = GspBackupData(
            version = 1,
            timestamp = timestamp,
            songs = songs,
            playlists = playlists,
            chains = chains,
            presets = presetsMap
        )

        return gson.toJson(backupData)
    }

    fun parseBackupJson(jsonString: String): BackupParseResult {
        if (jsonString.isBlank()) {
            return BackupParseResult(errorMessage = "Selected JSON file is empty")
        }
        return try {
            val type = object : TypeToken<GspBackupData>() {}.type
            val data = gson.fromJson<GspBackupData>(jsonString, type)
            if (data != null && (data.songs.isNotEmpty() || data.playlists.isNotEmpty() || data.chains.isNotEmpty() || data.presets.isNotEmpty())) {
                BackupParseResult(data = data)
            } else {
                BackupParseResult(errorMessage = "JSON file does not contain valid GSP backup structure")
            }
        } catch (e: Exception) {
            e.printStackTrace()
            BackupParseResult(errorMessage = e.localizedMessage ?: "Invalid JSON syntax")
        }
    }

    fun restoreBackup(backupData: GspBackupData, mode: ImportMode) {
        when (mode) {
            ImportMode.OVERWRITE -> {
                songManager.saveSongsDirectly(backupData.songs)
                playlistManager.savePlaylists(backupData.playlists)
                presetManager.saveChainsDirectly(backupData.chains)
                for ((effectId, presetsList) in backupData.presets) {
                    presetManager.savePresetsDirectly(effectId, presetsList)
                }
            }
            ImportMode.MERGE -> {
                mergeSongs(backupData.songs)
                mergePlaylists(backupData.playlists)
                mergeChains(backupData.chains)
                mergePresets(backupData.presets)
            }
        }
    }

    private fun mergeSongs(newSongs: List<Song>) {
        val currentSongs = songManager.loadSongs().toMutableList()
        var highestId = currentSongs.maxOfOrNull { it.id } ?: 0L

        for (song in newSongs) {
            val existing = currentSongs.firstOrNull { it.title.equals(song.title, ignoreCase = true) }
            if (existing == null) {
                highestId++
                currentSongs.add(song.copy(id = highestId))
            }
        }
        songManager.saveSongsDirectly(currentSongs)
    }

    private fun mergePlaylists(newPlaylists: List<Playlist>) {
        val currentPlaylists = playlistManager.loadPlaylists().toMutableList()
        var highestId = currentPlaylists.maxOfOrNull { it.id } ?: 0L

        for (pl in newPlaylists) {
            val existing = currentPlaylists.firstOrNull { it.name.equals(pl.name, ignoreCase = true) }
            if (existing == null) {
                highestId++
                currentPlaylists.add(pl.copy(id = highestId))
            }
        }
        playlistManager.savePlaylists(currentPlaylists)
    }

    private fun mergeChains(newChains: List<ChainPreset>) {
        val currentChains = presetManager.loadChains().toMutableList()
        var highestId = currentChains.maxOfOrNull { it.id } ?: 0L

        for (chain in newChains) {
            val existing = currentChains.firstOrNull { it.name.equals(chain.name, ignoreCase = true) }
            if (existing == null) {
                highestId++
                currentChains.add(chain.copy(id = highestId))
            }
        }
        presetManager.saveChainsDirectly(currentChains)
    }

    private fun mergePresets(newPresetsMap: Map<String, List<EffectPreset>>) {
        for ((effectId, newPresets) in newPresetsMap) {
            val currentPresets = presetManager.loadPresets(effectId).toMutableList()
            for (preset in newPresets) {
                if (currentPresets.none { it.name.equals(preset.name, ignoreCase = true) }) {
                    currentPresets.add(preset)
                }
            }
            presetManager.savePresetsDirectly(effectId, currentPresets)
        }
    }
}
