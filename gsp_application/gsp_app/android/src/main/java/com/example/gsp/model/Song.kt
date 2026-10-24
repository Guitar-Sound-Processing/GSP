package com.example.gsp.model

data class Song(
    var title: String,
    var notes: String = "",
    val id: Long,
    var chainId: Long
)
