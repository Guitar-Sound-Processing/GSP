package com.example.gsp.model

data class Playlist(
    val id: Long = 0L,
    val name: String,
    val songIds: List<Long> = emptyList()
)
