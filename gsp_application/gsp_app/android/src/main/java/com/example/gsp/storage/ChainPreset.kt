package com.example.gsp.storage

data class ChainPreset(
    val name: String,
    val id: Long,
    val commands: MutableList<String>
)
