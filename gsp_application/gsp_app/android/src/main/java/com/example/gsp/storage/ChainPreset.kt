package com.example.gsp.storage

data class ChainPreset(
    val id: Long,
    val name: String,
    val commands: MutableList<String>
)