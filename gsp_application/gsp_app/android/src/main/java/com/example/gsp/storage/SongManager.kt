package com.example.gsp.storage

import android.content.Context
import com.example.gsp.model.Song
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken
import androidx.core.content.edit

class SongManager(
    private val context: Context
) {

    private val prefs =
        context.getSharedPreferences(
            "songs",
            Context.MODE_PRIVATE
        )

    private val gsonCompact = Gson()

    private fun formatSongsJson(songs: List<Song>): String {
        if (songs.isEmpty()) return "[]"
        val lines = songs.joinToString(",\n") { "\t" + gsonCompact.toJson(it) }
        return "[\n$lines\n]"
    }

    /*
     * Carrega todas as músicas salvas.
     */
    fun loadSongs(): List<Song> {
        var rawJson = prefs.getString("songs", null)
        if (rawJson == null) {
            StorageInitializer(context).initializeDefaultDataIfNeeded()
            rawJson = prefs.getString("songs", null)
        }
        val type = object : TypeToken<List<Song>>() {}.type
        return if (rawJson.isNullOrEmpty()) emptyList() else gsonCompact.fromJson(rawJson, type)
    }

    /*
     * Salva uma música.
     */
    fun saveSong(song: Song) {
        val songs = loadSongs().toMutableList()
        val index = songs.indexOfFirst { it.id == song.id }
        if (index >= 0) {
            songs[index] = song
        } else {
            songs.add(song)
        }

        prefs.edit {
            putString("songs", formatSongsJson(songs))
        }
    }

    /*
     * Exclui uma música pelo ID.
     */
    fun deleteSong(id: Long) {
        val songs = loadSongs().filterNot { it.id == id }
        prefs.edit {
            putString("songs", formatSongsJson(songs))
        }
    }

    /*
     * Procura uma música pelo ID.
     */
    fun findSong(id: Long): Song? {
        return loadSongs().firstOrNull { it.id == id }
    }
}
