package com.example.gsp

import android.annotation.SuppressLint
import android.bluetooth.BluetoothManager
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import androidx.compose.foundation.layout.Column
import androidx.compose.runtime.*
import androidx.core.content.FileProvider
import com.example.gsp.bluetooth.BluetoothService
import com.example.gsp.model.ExpectedResponse
import com.example.gsp.model.Screen
import com.example.gsp.storage.BackupManager
import com.example.gsp.storage.GspBackupData
import com.example.gsp.ui.BackupScreen
import com.example.gsp.ui.ChainScreen
import com.example.gsp.ui.ExpPedalScreen
import com.example.gsp.ui.PedalScreen
import com.example.gsp.ui.PlayListScreen
import com.example.gsp.ui.SongScreen
import com.example.gsp.ui.TopBar
import com.example.gsp.viewmodel.MainViewModel
import java.io.File

class MainActivity : ComponentActivity() {

    private lateinit var bluetoothService: BluetoothService
    private val viewModel: MainViewModel by viewModels()

    private var pendingImportData by mutableStateOf<GspBackupData?>(null)
    private var importErrorMessage by mutableStateOf<String?>(null)

    // Launcher 1: Export Backup File (CreateDocument)
    private val exportFileLauncher = registerForActivityResult(
        ActivityResultContracts.CreateDocument("application/json")
    ) { uri: Uri? ->
        if (uri != null) {
            try {
                val backupManager = BackupManager(this)
                val jsonString = backupManager.createBackupJson()
                contentResolver.openOutputStream(uri)?.use { os ->
                    os.write(jsonString.toByteArray())
                }
                viewModel.addLog("Backup saved successfully")
            } catch (e: Exception) {
                e.printStackTrace()
                viewModel.addLog("Error saving backup: ${e.message}")
            }
        }
    }

    // Launcher 2: Select Import File (GetContent)
    private val importFileLauncher = registerForActivityResult(
        ActivityResultContracts.GetContent()
    ) { uri: Uri? ->
        if (uri != null) {
            try {
                val jsonString = contentResolver.openInputStream(uri)?.bufferedReader()?.use { it.readText() } ?: ""
                val backupManager = BackupManager(this)
                val parseResult = backupManager.parseBackupJson(jsonString)
                if (parseResult.data != null) {
                    pendingImportData = parseResult.data
                    importErrorMessage = null
                    viewModel.addLog("Backup file read successfully")
                } else {
                    pendingImportData = null
                    importErrorMessage = parseResult.errorMessage ?: "Failed to parse JSON file"
                    viewModel.addLog("Import error: $importErrorMessage")
                }
            } catch (e: Exception) {
                e.printStackTrace()
                pendingImportData = null
                importErrorMessage = "Error reading file: ${e.localizedMessage ?: e.message}"
                viewModel.addLog("Error reading import file: ${e.message}")
            }
        }
    }

    private fun shareBackupFile() {
        try {
            val backupManager = BackupManager(this)
            val jsonString = backupManager.createBackupJson()
            val cacheFile = File(cacheDir, "gsp_backup.json")
            cacheFile.writeText(jsonString)

            val fileUri: Uri = FileProvider.getUriForFile(
                this,
                "$packageName.provider",
                cacheFile
            )

            val sendIntent = Intent(Intent.ACTION_SEND).apply {
                type = "application/json"
                putExtra(Intent.EXTRA_STREAM, fileUri)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            }
            val shareIntent = Intent.createChooser(sendIntent, "Share GSP Backup").apply {
                addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            }
            startActivity(shareIntent)
        } catch (e: Exception) {
            e.printStackTrace()
            viewModel.addLog("Error sharing backup: ${e.message}")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 🔌 starting Bluetooth
        bluetoothService = BluetoothService()
        viewModel.setBluetoothService(bluetoothService)

        // 🔌 connect to bluetooth
        connectToGSP()

        setContent {

            // 🧭 nav state (Playlist screen as default initial screen)
            var currentScreen by remember { mutableStateOf(Screen.PLAYLISTS) }

            // 📡 connection state (LED)
            var isConnected by remember { mutableStateOf(false) }

            // 🔗 Join callbacks to Bluetooth (once)
            LaunchedEffect(Unit) {
                bluetoothService.onConnectionChanged = { connected ->
                    isConnected = connected
                    if (connected) {
                        viewModel.onBluetoothConnected()
                    } else {
                        viewModel.onBluetoothDisconnected()
                    }
                }
                bluetoothService.onMessageReceived = { msg ->
                    viewModel.addLog(msg)
                    viewModel.handleMessage(msg)
                }
            }

            // 🎛️ Main UI
            Column {

                // 🔝 TopBar (5 ícones com destaque do item ativo)
                TopBar(
                    isConnected = isConnected,
                    currentScreen = currentScreen,
                    onPresets = { currentScreen = Screen.PRESETS },
                    onChains = { currentScreen = Screen.CHAINS },
                    onSongs = { currentScreen = Screen.SONGS },
                    onPlaylists = {
                        viewModel.loadActivePlaylistFromStorage()
                        currentScreen = Screen.PLAYLISTS
                    },
                    onBackup = {
                        currentScreen = Screen.BACKUP
                    }
                )

                // 📱 Screens
                when (currentScreen) {

                    Screen.PLAYLISTS -> {
                        LaunchedEffect(Unit) {
                            viewModel.loadActivePlaylistFromStorage()
                        }
                        PlayListScreen(
                            playlists = viewModel.playlists,
                            currentPlaylistId = viewModel.currentPlaylistId,
                            currentPlaylistName = viewModel.currentPlaylistName,
                            playlistSongs = viewModel.playlistSongs,
                            allSongs = viewModel.songsList,
                            activeIndex = viewModel.activePlaylistItemIndex,
                            getChainName = { id -> viewModel.getChainNameById(id) },
                            onSelectPlaylist = { id -> viewModel.selectPlaylist(id) },
                            onCreatePlaylist = { name -> viewModel.createPlaylist(name) },
                            onDeletePlaylist = { id -> viewModel.deletePlaylist(id) },
                            onSelectSongAt = { idx -> viewModel.selectPlaylistSongAt(idx) },
                            onAddSong = { song -> viewModel.addSongToPlaylist(song) },
                            onRemoveSongAt = { idx -> viewModel.removeSongFromPlaylistAt(idx) },
                            onMoveUp = { idx -> viewModel.movePlaylistSongUp(idx) },
                            onMoveDown = { idx -> viewModel.movePlaylistSongDown(idx) },
                            onNextSong = { viewModel.nextPlaylistSong() },
                            onPrevSong = { viewModel.previousPlaylistSong() },
                            isConnected = isConnected,
                            onNavigate = { currentScreen = it }
                        )
                    }

                    Screen.SONGS -> SongScreen(
                        songs = viewModel.songsList,
                        availableChains = viewModel.getChains(),
                        currentSongId = viewModel.currentSongId,
                        getChainName = { id -> viewModel.getChainNameById(id) },
                        onSelectSong = { song -> viewModel.selectAndTransmitSong(song) },
                        onSaveSong = { id, title, chainName, notes -> viewModel.saveSong(id, title, chainName, notes) },
                        onDeleteSong = { id -> viewModel.deleteSong(id) },
                        isConnected = isConnected,
                        onNavigate = { currentScreen = it }
                    )

                    Screen.CHAINS -> ChainScreen(
                        chain = viewModel.chainState,
                        chainName = viewModel.currentChainName,
                        currentIndex = viewModel.currentChainIndex,
                        onChainNameChange = {
                            viewModel.updateCurrentChainName(it)
                        },
                        onCurrentIndexChange = {
                            viewModel.updateCurrentChainIndex(it)
                        },
                        effects = viewModel.effects,
                        sendCommand = { cmd, expected ->
                            viewModel.sendCommand(cmd, expected)
                        },
                        getChains = { viewModel.getChains() },
                        loadChain = { name -> viewModel.loadChain(name) },
                        saveChain = { name, chain -> viewModel.saveChain(name, chain) },
                        deleteChain = { name -> viewModel.deleteChain(name) },
                        onEditEffect = { id ->
                            viewModel.selectEffectById(id)
                            currentScreen = Screen.PRESETS
                        },
                        logText = viewModel.logText,
                        isConnected = isConnected,
                        showDebug = viewModel.showDebug,
                        onToggleDebug = { viewModel.toggleDebug() }
                    )

                    Screen.PRESETS -> PedalScreen(
                        effects = viewModel.getActiveChainEffects(),
                        currentIndex = viewModel.currentEffectIndex,
                        onNext = { viewModel.nextEffect() },
                        onPrev = { viewModel.prevEffect() },
                        onUpdateEffect = { viewModel.updateEffect(it) },
                        onSendEffect = { effect, write, expected ->
                            val cmd = viewModel.buildCommand(effect, write)
                            viewModel.sendCommand(cmd, expected)
                        },
                        onSendCommand = { cmd, expected ->
                            viewModel.sendCommand(cmd, expected)
                        },
                        onAddPreset = { effect, name ->
                            val cmd = viewModel.buildCommand(effect, true)
                            viewModel.savePreset(effect.id, name, cmd)
                        },
                        onSavePreset = { effect, name ->
                            val cmd = viewModel.buildCommand(effect, true)
                            viewModel.savePreset(effect.id, name, cmd)
                        },
                        onLoadPreset = { effect, name, expected ->
                            val cmd = viewModel.loadPreset(effect.id, name)
                            if (cmd != null) {
                                viewModel.sendCommand(cmd, expected)
                            }
                        },
                        onDeletePreset = { effect, name ->
                            viewModel.deletePreset(effect.id, name)
                        },
                        getPresets = { effectId ->
                            viewModel.getPresets(effectId)
                        },
                        logText = viewModel.logText,
                        isConnected = isConnected,
                        onNavigate = { currentScreen = it },
                        showDebug = viewModel.showDebug,
                        onToggleDebug = { viewModel.toggleDebug() }
                    )

                    Screen.EXPEDALS -> ExpPedalScreen(
                        log = viewModel.logText,
                        sendCommand = { cmd, expected ->
                            viewModel.sendCommand(cmd, expected)
                        },
                        isConnected = isConnected,
                        onNavigate = { currentScreen = it }
                    )

                    Screen.BACKUP -> BackupScreen(
                        isConnected = isConnected,
                        onExportFile = { exportFileLauncher.launch("gsp_backup.json") },
                        onShareBackup = { shareBackupFile() },
                        onPickImportFile = { importFileLauncher.launch("application/json") },
                        pendingImportData = pendingImportData,
                        importErrorMessage = importErrorMessage,
                        onConfirmImport = { data, mode ->
                            val backupManager = BackupManager(this@MainActivity)
                            backupManager.restoreBackup(data, mode)
                            viewModel.refreshAllDataFromStorage()
                            pendingImportData = null
                            importErrorMessage = null
                            viewModel.addLog("Backup imported successfully")
                        },
                        onCancelImport = {
                            pendingImportData = null
                            importErrorMessage = null
                        },
                        logText = viewModel.logText,
                        onNavigate = { currentScreen = it }
                    )
                }
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        bluetoothService.stopAutoConnect()
    }

    // 🔌 Bluetooth
    @SuppressLint("MissingPermission")
    private fun connectToGSP() {

        val bluetoothManager = getSystemService(BLUETOOTH_SERVICE) as BluetoothManager
        val adapter = bluetoothManager.adapter

        adapter?.cancelDiscovery()

        val deviceProvider = {
            adapter?.bondedDevices
                ?.firstOrNull { it.name?.contains("GSP", ignoreCase = true) == true }
        }

        bluetoothService.startAutoConnect(deviceProvider)
    }
}
