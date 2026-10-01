package com.example.gsp.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Add
import androidx.compose.material.icons.filled.ArrowDropDown
import androidx.compose.material.icons.filled.Delete
import androidx.compose.material.icons.filled.KeyboardArrowDown
import androidx.compose.material.icons.filled.KeyboardArrowUp
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.gsp.model.Playlist
import com.example.gsp.model.Song
import com.example.gsp.model.Screen

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun PlayListScreen(
    playlists: List<Playlist>,
    currentPlaylistId: Long,
    currentPlaylistName: String,
    playlistSongs: List<Song>,
    allSongs: List<Song>,
    activeIndex: Int,
    getChainName: (Long) -> String,
    onSelectPlaylist: (Long) -> Unit,
    onCreatePlaylist: (String) -> Unit,
    onDeletePlaylist: (Long) -> Unit,
    onSelectSongAt: (Int) -> Unit,
    onAddSong: (Song) -> Unit,
    onRemoveSongAt: (Int) -> Unit,
    onMoveUp: (Int) -> Unit,
    onMoveDown: (Int) -> Unit,
    onNextSong: () -> Unit,
    onPrevSong: () -> Unit,
    isConnected: Boolean,
    onNavigate: (Screen) -> Unit
) {
    var showAddSongDialog by remember { mutableStateOf(false) }
    var showCreatePlaylistDialog by remember { mutableStateOf(false) }
    var showDeletePlaylistDialog by remember { mutableStateOf(false) }
    var newPlaylistNameInput by remember { mutableStateOf("") }
    var playlistDropdownExpanded by remember { mutableStateOf(false) }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(12.dp)
    ) {
        // Top Header
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .padding(bottom = 6.dp)
        ) {
            Box(
                modifier = Modifier
                    .size(16.dp)
                    .background(
                        color = if (isConnected) Color.Green else Color.Red,
                        shape = CircleShape
                    )
                    .align(Alignment.CenterStart)
            )
            Text(
                text = "Playlist",
                style = MaterialTheme.typography.titleMedium.copy(
                    fontWeight = FontWeight.Bold
                ),
                modifier = Modifier.align(Alignment.Center)
            )
        }

        // Playlist Selection Header Row
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(vertical = 4.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.spacedBy(6.dp)
        ) {
            // Dropdown button to pick playlist
            Box(modifier = Modifier.weight(1f)) {
                OutlinedButton(
                    onClick = { playlistDropdownExpanded = true },
                    modifier = Modifier.fillMaxWidth(),
                    shape = RoundedCornerShape(6.dp),
                    contentPadding = PaddingValues(horizontal = 12.dp, vertical = 8.dp)
                ) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            text = currentPlaylistName,
                            fontSize = 15.sp,
                            fontWeight = FontWeight.Bold,
                            style = MaterialTheme.typography.bodyLarge
                        )
                        Icon(Icons.Default.ArrowDropDown, contentDescription = "Select Playlist")
                    }
                }

                DropdownMenu(
                    expanded = playlistDropdownExpanded,
                    onDismissRequest = { playlistDropdownExpanded = false },
                    modifier = Modifier.fillMaxWidth(0.85f)
                ) {
                    playlists.forEach { pl ->
                        DropdownMenuItem(
                            text = {
                                Text(
                                    text = pl.name,
                                    fontSize = 15.sp,
                                    fontWeight = if (pl.id == currentPlaylistId) FontWeight.Bold else FontWeight.Normal,
                                    style = MaterialTheme.typography.bodyLarge
                                )
                            },
                            onClick = {
                                onSelectPlaylist(pl.id)
                                playlistDropdownExpanded = false
                            }
                        )
                    }
                }
            }

            // Create New Playlist Button
            IconButton(
                onClick = {
                    newPlaylistNameInput = ""
                    showCreatePlaylistDialog = true
                },
                modifier = Modifier
                    .size(42.dp)
                    .background(MaterialTheme.colorScheme.primaryContainer, shape = RoundedCornerShape(6.dp))
            ) {
                Icon(
                    Icons.Default.Add,
                    contentDescription = "New Playlist",
                    tint = MaterialTheme.colorScheme.onPrimaryContainer
                )
            }

            // Delete Current Playlist Button
            IconButton(
                onClick = { showDeletePlaylistDialog = true },
                enabled = playlists.size > 1,
                modifier = Modifier
                    .size(42.dp)
                    .background(
                        if (playlists.size > 1) MaterialTheme.colorScheme.errorContainer else Color.LightGray,
                        shape = RoundedCornerShape(6.dp)
                    )
            ) {
                Icon(
                    Icons.Default.Delete,
                    contentDescription = "Delete Playlist",
                    tint = if (playlists.size > 1) Color.Red else Color.Gray
                )
            }
        }

        // Add Song Trigger Button
        Button(
            onClick = { showAddSongDialog = true },
            colors = ButtonDefaults.buttonColors(containerColor = MaterialTheme.colorScheme.secondary),
            modifier = Modifier
                .fillMaxWidth()
                .height(42.dp)
                .padding(vertical = 2.dp),
            shape = RoundedCornerShape(6.dp)
        ) {
            Text(
                text = "+ Add Song to Playlist",
                fontSize = 14.sp,
                fontWeight = FontWeight.Bold,
                style = MaterialTheme.typography.labelLarge
            )
        }

        Spacer(modifier = Modifier.height(4.dp))

        // Playlist View
        LazyColumn(
            modifier = Modifier.weight(1f),
            verticalArrangement = Arrangement.spacedBy(6.dp)
        ) {
            itemsIndexed(playlistSongs) { index, song ->
                val isActive = index == activeIndex
                val chainName = getChainName(song.chainId)

                Card(
                    modifier = Modifier
                        .fillMaxWidth()
                        .clickable { onSelectSongAt(index) },
                    shape = RoundedCornerShape(8.dp),
                    colors = CardDefaults.cardColors(
                        containerColor = if (isActive) MaterialTheme.colorScheme.primaryContainer 
                                         else MaterialTheme.colorScheme.surfaceVariant
                    ),
                    elevation = CardDefaults.cardElevation(defaultElevation = if (isActive) 4.dp else 1.dp)
                ) {
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(horizontal = 10.dp, vertical = 6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        // Sequence Order Tracker Number
                        Text(
                            text = "${index + 1}.",
                            fontSize = 18.sp,
                            fontWeight = FontWeight.Bold,
                            color = if (isActive) MaterialTheme.colorScheme.onPrimaryContainer else Color.Gray,
                            style = MaterialTheme.typography.bodyLarge,
                            modifier = Modifier.padding(end = 8.dp)
                        )

                        // Meta details
                        Column(modifier = Modifier.weight(1f)) {
                            Text(
                                text = song.title,
                                fontSize = 18.sp,
                                fontWeight = FontWeight.Bold,
                                color = if (isActive) MaterialTheme.colorScheme.onPrimaryContainer 
                                        else MaterialTheme.colorScheme.onSurfaceVariant,
                                style = MaterialTheme.typography.bodyLarge
                            )
                            Text(
                                text = "Chain: $chainName",
                                fontSize = 13.sp,
                                color = if (isActive) MaterialTheme.colorScheme.onPrimaryContainer.copy(alpha = 0.75f)
                                        else Color.Gray,
                                style = MaterialTheme.typography.bodySmall
                            )
                        }

                        // Reordering Up / Down Control arrows & Delete Icon
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(2.dp)
                        ) {
                            IconButton(
                                onClick = { onMoveUp(index) },
                                enabled = index > 0,
                                modifier = Modifier.size(34.dp)
                            ) {
                                Icon(Icons.Default.KeyboardArrowUp, contentDescription = "Move Up")
                            }
                            IconButton(
                                onClick = { onMoveDown(index) },
                                enabled = index < playlistSongs.size - 1,
                                modifier = Modifier.size(34.dp)
                            ) {
                                Icon(Icons.Default.KeyboardArrowDown, contentDescription = "Move Down")
                            }
                            IconButton(
                                onClick = { onRemoveSongAt(index) },
                                modifier = Modifier.size(34.dp)
                            ) {
                                Icon(Icons.Default.Delete, contentDescription = "Remove", tint = Color.Red)
                            }
                        }
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(6.dp))

        // Performance Next & Previous Buttons
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(vertical = 4.dp),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            OutlinedButton(
                onClick = onPrevSong,
                modifier = Modifier
                    .weight(1f)
                    .height(54.dp),
                shape = RoundedCornerShape(8.dp)
            ) {
                Text(
                    text = "Previous",
                    fontSize = 16.sp,
                    fontWeight = FontWeight.Bold,
                    style = MaterialTheme.typography.labelLarge
                )
            }
            
            Button(
                onClick = onNextSong,
                modifier = Modifier
                    .weight(1.5f)
                    .height(54.dp),
                colors = ButtonDefaults.buttonColors(containerColor = MaterialTheme.colorScheme.primary),
                shape = RoundedCornerShape(8.dp)
            ) {
                Text(
                    text = "Next Song ➔",
                    fontSize = 18.sp,
                    fontWeight = FontWeight.Bold,
                    style = MaterialTheme.typography.labelLarge
                )
            }
        }
    }

    // Modal Dialog to Select and Insert a Song from existing Repository
    if (showAddSongDialog) {
        AlertDialog(
            onDismissRequest = { showAddSongDialog = false },
            title = { Text("Select Song to Add", fontWeight = FontWeight.Bold, fontSize = 18.sp, style = MaterialTheme.typography.titleMedium) },
            text = {
                if (allSongs.isEmpty()) {
                    Text("No songs found in your library. Add songs in Songs screen first.", fontSize = 15.sp, style = MaterialTheme.typography.bodyMedium)
                } else {
                    LazyColumn(
                        modifier = Modifier
                            .fillMaxWidth()
                            .heightIn(max = 300.dp),
                        verticalArrangement = Arrangement.spacedBy(6.dp)
                    ) {
                        itemsIndexed(allSongs) { _, song ->
                            Card(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .clickable {
                                        onAddSong(song)
                                        showAddSongDialog = false
                                    },
                                shape = RoundedCornerShape(6.dp),
                                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant)
                            ) {
                                Text(
                                    text = song.title,
                                    fontSize = 16.sp,
                                    fontWeight = FontWeight.Bold,
                                    style = MaterialTheme.typography.bodyLarge,
                                    modifier = Modifier.padding(12.dp)
                                )
                            }
                        }
                    }
                }
            },
            confirmButton = {},
            dismissButton = {
                TextButton(onClick = { showAddSongDialog = false }) {
                    Text("Close", style = MaterialTheme.typography.labelLarge)
                }
            }
        )
    }

    // Modal Dialog to Create a New Playlist
    if (showCreatePlaylistDialog) {
        AlertDialog(
            onDismissRequest = { showCreatePlaylistDialog = false },
            title = { Text("New Playlist", fontWeight = FontWeight.Bold, fontSize = 18.sp, style = MaterialTheme.typography.titleMedium) },
            text = {
                OutlinedTextField(
                    value = newPlaylistNameInput,
                    onValueChange = { newPlaylistNameInput = it },
                    label = { Text("Playlist Name", style = MaterialTheme.typography.bodyMedium) },
                    singleLine = true,
                    modifier = Modifier.fillMaxWidth()
                )
            },
            confirmButton = {
                Button(
                    onClick = {
                        if (newPlaylistNameInput.isNotBlank()) {
                            onCreatePlaylist(newPlaylistNameInput.trim())
                            showCreatePlaylistDialog = false
                        }
                    }
                ) {
                    Text("Create", fontWeight = FontWeight.Bold, style = MaterialTheme.typography.labelLarge)
                }
            },
            dismissButton = {
                TextButton(onClick = { showCreatePlaylistDialog = false }) {
                    Text("Cancel", style = MaterialTheme.typography.labelLarge)
                }
            }
        )
    }

    // Modal Dialog to Confirm Deleting Current Playlist
    if (showDeletePlaylistDialog) {
        AlertDialog(
            onDismissRequest = { showDeletePlaylistDialog = false },
            title = { Text("Delete Playlist", fontWeight = FontWeight.Bold, fontSize = 18.sp, style = MaterialTheme.typography.titleMedium) },
            text = {
                Text(
                    text = "Are you sure you want to delete '$currentPlaylistName'?",
                    fontSize = 15.sp,
                    style = MaterialTheme.typography.bodyMedium
                )
            },
            confirmButton = {
                Button(
                    onClick = {
                        onDeletePlaylist(currentPlaylistId)
                        showDeletePlaylistDialog = false
                    },
                    colors = ButtonDefaults.buttonColors(containerColor = MaterialTheme.colorScheme.error)
                ) {
                    Text("Delete", fontWeight = FontWeight.Bold, style = MaterialTheme.typography.labelLarge)
                }
            },
            dismissButton = {
                TextButton(onClick = { showDeletePlaylistDialog = false }) {
                    Text("Cancel", style = MaterialTheme.typography.labelLarge)
                }
            }
        )
    }
}
