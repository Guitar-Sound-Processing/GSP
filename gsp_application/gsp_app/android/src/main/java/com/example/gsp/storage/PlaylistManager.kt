package com.example.gsp.storage

import android.content.Context
import com.example.gsp.model.Playlist
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken
import androidx.core.content.edit

class PlaylistManager(private val context: Context) {

    private val prefs = context.getSharedPreferences("playlists", Context.MODE_PRIVATE)
    private val gsonCompact = Gson()

    private fun formatPlaylistsJson(playlists: List<Playlist>): String {
        if (playlists.isEmpty()) return "[]"
        val lines = playlists.joinToString(",\n") { "\t" + gsonCompact.toJson(it) }
        return "[\n$lines\n]"
    }

    fun loadPlaylists(): List<Playlist> {
        var rawJson = prefs.getString("playlists", null)
        if (rawJson == null) {
            StorageInitializer(context).initializeDefaultDataIfNeeded()
            rawJson = prefs.getString("playlists", null)
        }
        val type = object : TypeToken<List<Playlist>>() {}.type
        val lists: List<Playlist>? = if (rawJson.isNullOrEmpty()) null else gsonCompact.fromJson(rawJson, type)
        return if (lists.isNullOrEmpty()) listOf(Playlist(id = 1L, name = "Default Setlist")) else lists
    }

    fun savePlaylists(playlists: List<Playlist>) {
        prefs.edit {
            putString("playlists", formatPlaylistsJson(playlists))
        }
    }
}
