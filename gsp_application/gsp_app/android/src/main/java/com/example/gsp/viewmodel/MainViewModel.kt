package com.example.gsp.viewmodel

import androidx.compose.runtime.*
import com.example.gsp.model.*
import com.example.gsp.storage.EffectPreset
import com.example.gsp.storage.PresetManager
import android.app.Application
import androidx.lifecycle.AndroidViewModel
import com.example.gsp.bluetooth.BluetoothService
import com.example.gsp.storage.ChainManager
import com.example.gsp.storage.SongManager
import com.example.gsp.storage.PlaylistManager
import com.example.gsp.storage.StorageInitializer
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue

class MainViewModel(application: Application) : AndroidViewModel(application) {

    // ---------------------------------------------------------------------
    // EFFECTS
    // ---------------------------------------------------------------------
    var effects by mutableStateOf(loadInitialEffects())
        private set
    var currentEffectIndex by mutableStateOf(0)
        private set

    // ---------------------------------------------------------------------
    // LOG (Refatorado para usar uma única lista thread-safe limitada a 10 itens)
    // ---------------------------------------------------------------------
    private var _logList by mutableStateOf(listOf<String>())
    val logText: String
        get() = _logList.joinToString("\n")

    // ---------------------------------------------------------------------
    // DEBUG
    // ---------------------------------------------------------------------
    var showDebug by mutableStateOf(false)
        private set

    // ---------------------------------------------------------------------
    // CURRENT CHAIN (Garantido o encapsulamento com private set)
    // ---------------------------------------------------------------------
    var currentChainName by mutableStateOf("")
        private set
    var currentChainIndex by mutableStateOf(0)
        private set
    var currentChainId by mutableLongStateOf(-1L)
        private set
    var chainState by mutableStateOf(listOf<String>())
        private set

    // ---------------------------------------------------------------------
    // SONG STATE (Garantido o encapsulamento com private set)
    // ---------------------------------------------------------------------
    var songsList by mutableStateOf(listOf<Song>())
        private set
    var currentSongIndex by mutableIntStateOf(0)
        private set
    var currentSongId by mutableLongStateOf(-1L)
        private set

    // ---------------------------------------------------------------------
    // STORAGE
    // ---------------------------------------------------------------------
    private val presetManager = PresetManager(application)
    private val chainManager = ChainManager(application)
    private val songManager = SongManager(application)
    private val playlistManager = PlaylistManager(application)

    // ---------------------------------------------------------------------
    // PLAYLIST STATE
    // ---------------------------------------------------------------------
    var playlists by mutableStateOf(listOf<Playlist>())
        private set
    var currentPlaylistId by mutableLongStateOf(1L)
        private set
    var currentPlaylistName by mutableStateOf("Default Setlist")
        private set
    var playlistSongs by mutableStateOf(listOf<Song>())
        private set
    var activePlaylistItemIndex by mutableIntStateOf(-1)
        private set

    init {
        // Carrega dados iniciais dos assets na primeira execução
        StorageInitializer(application).initializeDefaultDataIfNeeded()

        // Inicializa a lista de músicas APÓS os gerenciadores estarem instanciados
        loadSongsFromStorage()
        loadActivePlaylistFromStorage()
    }

    // ---------------------------------------------------------------------
    // BLUETOOTH / EXPECTED RESPONSES (Fila protegida contra concorrência)
    // ---------------------------------------------------------------------
    private val expectedResponses = ArrayDeque<ExpectedResponse>()
    private var bluetoothService: BluetoothService? = null

    // =====================================================================
    // BLUETOOTH
    // =====================================================================
    fun setBluetoothService(service: BluetoothService) {
        bluetoothService = service
    }

    fun onBluetoothConnected() {
        addLog("System: Connected to GSP")
        sendCommand("all", ExpectedResponse.CHAIN)
    }

    fun onBluetoothDisconnected() {
        addLog("System: Disconnected from GSP")
    }

    fun sendCommand(
        cmd: String,
        expected: ExpectedResponse
    ) {
        if (bluetoothService?.isConnected == true) {
            synchronized(expectedResponses) {
                expectedResponses.addLast(expected)
            }
            addLog("<- $cmd")
            bluetoothService?.send(cmd)
        } else {
            addLog("<- [OFFLINE] $cmd")
            println("Offline mode: command skipped -> $cmd")
        }
    }

    // =====================================================================
    // DEBUG
    // =====================================================================
    fun toggleDebug() {
        showDebug = !showDebug
    }

    // =====================================================================
    // EFFECT NAVIGATION
    // =====================================================================
    fun nextEffect() {
        currentEffectIndex = (currentEffectIndex + 1) % effects.size
    }

    fun prevEffect() {
        currentEffectIndex = (currentEffectIndex - 1 + effects.size) % effects.size
    }

    fun updateEffect(updated: Effect) {
        effects = effects.map {
            if (it.id == updated.id) updated else it
        }
    }

    // =====================================================================
    // LOG
    // =====================================================================
    fun addLog(message: String) {
        // takeLast(10) garante tamanho máximo fixo, eliminando os logs antigos automaticamente
        _logList = (_logList + message).takeLast(10)
    }

    // =====================================================================
    // UPDATE EFFECT FROM DAISY
    // =====================================================================
    fun updateEffectFromDaisy(
        id: String,
        position: Int,
        enabled: Boolean,
        values: List<Float>
    ) {
        effects = effects.map { effect ->
            if (effect.id != id) {
                return@map effect
            }
            val valueIterator = values.iterator()
            val updatedParams = effect.parameters.map { param ->
                if (param.type == EffectParameter.ParamType.LFO && param.lfo != null) {
                    val lfo = param.lfo
                    if (!valueIterator.hasNext()) return@map param
                    val waveform = valueIterator.next().toInt()
                    
                    if (!valueIterator.hasNext()) return@map param
                    val freq = valueIterator.next()
                    
                    if (!valueIterator.hasNext()) return@map param
                    val duty = valueIterator.next()
                    
                    param.copy(
                        lfo = LfoParameter(
                            waveform = waveform,
                            frequency = lfo.frequency.copy(
                                value = lfo.frequency.fromMappedValue(freq)
                            ),
                            dutyCycle = lfo.dutyCycle.copy(
                                value = lfo.dutyCycle.fromMappedValue(duty)
                            ),
                            pedalChannel = lfo.pedalChannel
                        )
                    )
                } else {
                    if (!valueIterator.hasNext()) return@map param
                    val v = valueIterator.next()
                    param.copy(
                        value = param.fromMappedValue(v)
                    )
                }
            }
            effect.copy(
                position = position,
                enabled = enabled,
                parameters = updatedParams
            )
        }
    }

    // =====================================================================
    // EXPRESSION PEDAL
    // =====================================================================
    fun buildCommand(effect: Effect, write: Boolean): String {
        return DaisyProtocolHandler.buildCommand(effect, write)
    }

    fun buildPotCommand(effect: Effect): String? {
        return DaisyProtocolHandler.buildPotCommand(effect)
    }

    fun updatePedalChannels(channels: Map<String, Int>) {
        effects = effects.map { effect ->
            val channel = channels[effect.id] ?: return@map effect
            val updatedParams = effect.parameters.map { param ->
                if (param.type == EffectParameter.ParamType.LFO && param.lfo != null) {
                    param.copy(lfo = param.lfo.copy(pedalChannel = channel))
                } else {
                    param
                }
            }
            effect.copy(parameters = updatedParams)
        }
    }

    // =====================================================================
    // EFFECT PRESETS
    // =====================================================================
    fun savePreset(
        effectId: String,
        name: String,
        command: String
    ) {
        val preset = EffectPreset(effectId = effectId, name = name, command = command)
        presetManager.savePreset(effectId, preset)
    }

    fun loadPreset(effectId: String, name: String): String? {
        return presetManager.loadPresets(effectId)
            .find { it.name == name }
            ?.command
    }

    fun getPresets(effectId: String): List<EffectPreset> {
        return presetManager.loadPresets(effectId)
    }

    fun deletePreset(effectId: String, name: String) {
        presetManager.deletePreset(effectId, name)
    }

    // =====================================================================
    // CHAIN STATE
    // =====================================================================
    fun updateCurrentChainName(name: String) {
        currentChainName = name
    }

    fun updateCurrentChainIndex(index: Int) {
        currentChainIndex = index
    }

    // =====================================================================
    // CHAIN STORAGE
    // =====================================================================
    fun getChains(): List<String> {
        return chainManager.getChains().map { it.name }
    }

    fun loadChain(name: String): List<String>? {
        val preset = chainManager.findByName(name) ?: return null
        currentChainId = preset.id
        currentChainName = preset.name
        return preset.commands
    }

    fun saveChain(name: String, chain: List<String>) {
        val commands = mutableListOf<String>()
        commands.add(DaisyProtocolHandler.CMD_CLEAR)
        
        val effectsInChain = chain.mapNotNull { id ->
            effects.firstOrNull { it.id == id }
        }
        
        chain.forEachIndexed { index, id ->
            val effect = effects.firstOrNull { it.id == id } ?: return@forEachIndexed
            val cmd = DaisyProtocolHandler.buildCommand(effect, true)
            
            if (id == DaisyProtocolHandler.CMD_LVD) {
                val parts = cmd.split(" ")
                if (parts.size >= 4) {
                    commands.add("${DaisyProtocolHandler.CMD_LVD} ${parts.drop(2).joinToString(" ")}")
                }
            } else {
                commands.add("$id ($index) ${cmd.substringAfter(id).trim()}")
            }
        }
        
        effectsInChain.forEach { effect ->
            DaisyProtocolHandler.buildPotCommand(effect)?.let { commands.add(it) }
        }

        val preset = chainManager.saveChain(name = name, commands = commands)
        currentChainId = preset.id
        currentChainName = preset.name
    }

    fun deleteChain(name: String) {
        val deleted = chainManager.deleteByName(name)
        if (deleted && currentChainName == name) {
            currentChainId = -1L
            currentChainName = DaisyProtocolHandler.DEFAULT_CHAIN_NAME
        }
    }

    // =====================================================================
    // HANDLE DAISY MESSAGE
    // =====================================================================
    fun handleMessage(msg: String) {
        val expected = synchronized(expectedResponses) {
            expectedResponses.removeFirstOrNull()
        }
        println("Received: $msg * $expected")
        
        when (expected) {
            ExpectedResponse.CHAIN -> {
                val chain = DaisyProtocolHandler.parseChain(msg)
                chainState = chain
                effects = effects.map { effect ->
                    val pos = chain.indexOfFirst { it.equals(effect.id, ignoreCase = true) }
                    effect.copy(position = if (pos >= 0) pos + 1 else -1)
                }
            }
            ExpectedResponse.EFFECT -> {
                val parsed = DaisyProtocolHandler.parseMessage(msg)
                parsed?.let { (id, pair, values) ->
                    val (position, enabled) = pair
                    updateEffectFromDaisy(
                        id = id,
                        position = position,
                        enabled = enabled,
                        values = values
                    )
                }
            }
            ExpectedResponse.POT -> {
                val channels = DaisyProtocolHandler.parsePot(msg)
                if (channels != null) {
                    updatePedalChannels(channels)
                    println("POT PARSED = $channels")
                }
            }
            ExpectedResponse.NONE -> return
            null -> println("Unknown DS reply: $msg")
            else -> println("Unknown DS reply: $msg")
        }
    }

    // =====================================================================
    // SELECT EFFECT
    // =====================================================================
    fun selectEffectById(id: String) {
        val idx = effects.indexOfFirst { it.id == id }
        if (idx >= 0) {
            currentEffectIndex = idx
        }
    }

    // =====================================================================
    // SONGS MANAGEMENT
    // =====================================================================
    fun loadSongsFromStorage() {
        songsList = songManager.loadSongs()
    }

    fun saveSong(id: Long, title: String, chainName: String) {
        val songId = if (id == 0L) {
            (songsList.maxOfOrNull { it.id } ?: 0L) + 1L
        } else {
            id
        }
        
        // Busca o ID da cadeia pelo nome fornecido
        val chainId = chainManager.findByName(chainName)?.id ?: -1L
        
        val song = Song(id = songId, title = title, chainId = chainId)
        songManager.saveSong(song)
        loadSongsFromStorage() // Atualiza a lista de músicas
        loadActivePlaylistFromStorage() // Atualiza os títulos e dados das músicas na playlist ativa
    }

    fun deleteSong(id: Long) {
        songManager.deleteSong(id)
        loadSongsFromStorage() // Atualiza a lista de músicas
        loadActivePlaylistFromStorage() // Atualiza os dados da playlist ativa
    }

    fun getChainNameById(chainId: Long): String {
        return chainManager.findById(chainId)?.name ?: "No Chain"
    }

    /**
     * Aciona uma música no palco. Localiza a cadeia de efeitos vinculada e transmite
     * imediatamente todos os comandos sequenciais ao Daisy Seed via Bluetooth.
     */
    fun selectAndTransmitSong(song: Song) {
        currentSongId = song.id
        val chainPreset = chainManager.findById(song.chainId)
        if (chainPreset != null) {
            currentChainId = chainPreset.id
            currentChainName = chainPreset.name
            
            // Transmite todos os comandos gravados da cadeia sequencialmente
            chainPreset.commands.forEach { cmd ->
                val expected = if (cmd.startsWith("pot")) ExpectedResponse.POT else ExpectedResponse.EFFECT
                sendCommand(cmd, expected)
            }
            // Solicita a confirmação do estado atual da cadeia ao Daisy Seed
            sendCommand("all", ExpectedResponse.CHAIN)
        }
    }

    // =====================================================================
    // PLAYLIST MANAGEMENT
    // =====================================================================
    fun loadActivePlaylistFromStorage() {
        val lists = playlistManager.loadPlaylists()
        playlists = if (lists.isEmpty()) listOf(Playlist(id = 1L, name = "Default Setlist")) else lists
        
        val activeList = playlists.firstOrNull { it.id == currentPlaylistId } ?: playlists.first()
        currentPlaylistId = activeList.id
        currentPlaylistName = activeList.name
        
        updatePlaylistSongsFromStorage(activeList)
    }

    private fun updatePlaylistSongsFromStorage(playlist: Playlist) {
        playlistSongs = playlist.songIds.mapNotNull { id ->
            songsList.firstOrNull { song -> song.id == id } ?: songManager.findSong(id)
        }
        if (activePlaylistItemIndex >= playlistSongs.size) {
            activePlaylistItemIndex = -1
        }
    }

    private fun autoSaveActivePlaylist() {
        val updatedSongIds = playlistSongs.map { it.id }
        val updatedLists = playlists.map { pl ->
            if (pl.id == currentPlaylistId) {
                pl.copy(songIds = updatedSongIds)
            } else {
                pl
            }
        }
        playlists = updatedLists
        playlistManager.savePlaylists(updatedLists)
    }

    fun selectPlaylist(id: Long) {
        val selected = playlists.firstOrNull { it.id == id } ?: return
        currentPlaylistId = selected.id
        currentPlaylistName = selected.name
        activePlaylistItemIndex = -1
        updatePlaylistSongsFromStorage(selected)
    }

    fun createPlaylist(name: String) {
        val newId = (playlists.maxOfOrNull { it.id } ?: 0L) + 1L
        val newPlaylist = Playlist(id = newId, name = name, songIds = emptyList())
        playlists = playlists + newPlaylist
        currentPlaylistId = newId
        currentPlaylistName = name
        activePlaylistItemIndex = -1
        playlistSongs = emptyList()
        playlistManager.savePlaylists(playlists)
    }

    fun deletePlaylist(id: Long) {
        val remaining = playlists.filter { it.id != id }
        val finalLists = if (remaining.isEmpty()) {
            listOf(Playlist(id = 1L, name = "Default Setlist"))
        } else {
            remaining
        }
        playlists = finalLists
        playlistManager.savePlaylists(finalLists)

        if (currentPlaylistId == id) {
            val first = finalLists.first()
            currentPlaylistId = first.id
            currentPlaylistName = first.name
            activePlaylistItemIndex = -1
            updatePlaylistSongsFromStorage(first)
        }
    }

    fun addSongToPlaylist(song: Song) {
        playlistSongs = playlistSongs + song
        autoSaveActivePlaylist()
    }

    fun removeSongFromPlaylistAt(index: Int) {
        if (index in playlistSongs.indices) {
            if (activePlaylistItemIndex == index) {
                activePlaylistItemIndex = -1
            } else if (activePlaylistItemIndex > index) {
                activePlaylistItemIndex--
            }
            playlistSongs = playlistSongs.filterIndexed { i, _ -> i != index }
            autoSaveActivePlaylist()
        }
    }

    fun selectPlaylistSongAt(index: Int) {
        if (index in playlistSongs.indices) {
            activePlaylistItemIndex = index
            selectAndTransmitSong(playlistSongs[index])
        }
    }

    fun nextPlaylistSong() {
        if (playlistSongs.isNotEmpty()) {
            val nextIndex = (activePlaylistItemIndex + 1) % playlistSongs.size
            selectPlaylistSongAt(nextIndex)
        }
    }

    fun previousPlaylistSong() {
        if (playlistSongs.isNotEmpty()) {
            val prevIndex = if (activePlaylistItemIndex <= 0) playlistSongs.size - 1 else activePlaylistItemIndex - 1
            selectPlaylistSongAt(prevIndex)
        }
    }

    fun movePlaylistSongUp(index: Int) {
        if (index > 0 && index in playlistSongs.indices) {
            val mutableList = playlistSongs.toMutableList()
            val temp = mutableList[index]
            mutableList[index] = mutableList[index - 1]
            mutableList[index - 1] = temp
            
            // Corrige o índice do item destacado se ele foi movido
            if (activePlaylistItemIndex == index) {
                activePlaylistItemIndex = index - 1
            } else if (activePlaylistItemIndex == index - 1) {
                activePlaylistItemIndex = index
            }
            
            playlistSongs = mutableList
            autoSaveActivePlaylist()
        }
    }

    fun movePlaylistSongDown(index: Int) {
        if (index >= 0 && index < playlistSongs.size - 1) {
            val mutableList = playlistSongs.toMutableList()
            val temp = mutableList[index]
            mutableList[index] = mutableList[index + 1]
            mutableList[index + 1] = temp
            
            // Corrige o índice do item destacado se ele foi movido
            if (activePlaylistItemIndex == index) {
                activePlaylistItemIndex = index + 1
            } else if (activePlaylistItemIndex == index + 1) {
                activePlaylistItemIndex = index
            }
            
            playlistSongs = mutableList
            autoSaveActivePlaylist()
        }
    }
}
