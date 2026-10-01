package com.example.gsp.model

data class Song(
    val id: Long,
    var title: String,
    var chainId: Long,
    var notes: String = ""
)