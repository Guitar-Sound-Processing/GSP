package com.example.gsp.model

data class Playlist(
    val name: String,
    val id: Long = 0L,
    val songIds: List<Long> = emptyList()
)
