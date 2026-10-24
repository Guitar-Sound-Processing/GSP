package com.example.gsp.storage

import com.example.gsp.model.Playlist
import com.example.gsp.model.Song

data class GspBackupData(
    val version: Int = 1,
    val timestamp: String = "",
    val songs: List<Song> = emptyList(),
    val playlists: List<Playlist> = emptyList(),
    val chains: List<ChainPreset> = emptyList(),
    val presets: Map<String, List<EffectPreset>> = emptyMap()
)
