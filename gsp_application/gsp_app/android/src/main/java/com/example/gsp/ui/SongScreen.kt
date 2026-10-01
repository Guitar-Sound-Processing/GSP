package com.example.gsp.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Delete
import androidx.compose.material.icons.filled.Edit
import androidx.compose.material.icons.filled.Search
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.gsp.model.Song
import com.example.gsp.model.Screen

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SongScreen(
    songs: List<Song>,
    availableChains: List<String>,
    currentSongId: Long,
    getChainName: (Long) -> String,
    onSelectSong: (Song) -> Unit,
    onSaveSong: (Long, String, String) -> Unit,
    onDeleteSong: (Long) -> Unit,
    isConnected: Boolean,
    onNavigate: (Screen) -> Unit
) {
    var searchQuery by remember { mutableStateOf("") }
    var isSortAscending by remember { mutableStateOf(true) }
    
    // Dialog UI state
    var showDialog by remember { mutableStateOf(false) }
    var editingSongId by remember { mutableStateOf(0L) }
    var inputTitle by remember { mutableStateOf("") }
    var selectedChainName by remember { mutableStateOf("") }
    var dropdownExpanded by remember { mutableStateOf(false) }

    // List filtering and ordering
    val filteredSongs = songs
        .filter { it.title.contains(searchQuery, ignoreCase = true) }
        .sortedWith { s1, s2 ->
            if (isSortAscending) s1.title.compareTo(s2.title, ignoreCase = true)
            else s2.title.compareTo(s1.title, ignoreCase = true)
        }

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
                text = "Songs",
                style = MaterialTheme.typography.titleMedium.copy(
                    fontWeight = FontWeight.Bold
                ),
                modifier = Modifier.align(Alignment.Center)
            )
        }

        // Search Field and Sort Button (Optimized Compact Layout)
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(vertical = 4.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.spacedBy(6.dp)
        ) {
            OutlinedTextField(
                value = searchQuery,
                onValueChange = { searchQuery = it },
                label = { Text("Search Song", fontSize = 14.sp) },
                leadingIcon = { Icon(Icons.Default.Search, contentDescription = "Search", modifier = Modifier.size(20.dp)) },
                modifier = Modifier.weight(1f),
                singleLine = true,
                shape = RoundedCornerShape(6.dp)
            )

            Button(
                onClick = { isSortAscending = !isSortAscending },
                contentPadding = PaddingValues(horizontal = 10.dp),
                shape = RoundedCornerShape(6.dp),
                modifier = Modifier.height(50.dp)
            ) {
                Text(if (isSortAscending) "A-Z ↑" else "Z-A ↓", fontSize = 14.sp, fontWeight = FontWeight.Bold)
            }
        }

        // Add Song Button
        Button(
            onClick = {
                editingSongId = 0L
                inputTitle = ""
                selectedChainName = availableChains.firstOrNull() ?: ""
                showDialog = true
            },
            colors = ButtonDefaults.buttonColors(containerColor = MaterialTheme.colorScheme.primary),
            modifier = Modifier
                .fillMaxWidth()
                .height(42.dp)
                .padding(vertical = 2.dp),
            shape = RoundedCornerShape(6.dp)
        ) {
            Text("+ Add Song", fontSize = 14.sp, fontWeight = FontWeight.Bold)
        }

        Spacer(modifier = Modifier.height(4.dp))

        // Songs List (Compact layout to minimize scrolling)
        LazyColumn(
            modifier = Modifier.weight(1f),
            verticalArrangement = Arrangement.spacedBy(6.dp)
        ) {
            items(filteredSongs) { song ->
                val isActive = song.id == currentSongId
                val linkedChainName = getChainName(song.chainId)

                Card(
                    modifier = Modifier
                        .fillMaxWidth()
                        .clickable { onSelectSong(song) },
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
                            .padding(horizontal = 12.dp, vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        // Title and Chain metadata in highly readable compact styling
                        Column(modifier = Modifier.weight(1f)) {
                            Text(
                                text = song.title,
                                fontSize = 18.sp,
                                fontWeight = FontWeight.Bold,
                                color = if (isActive) MaterialTheme.colorScheme.onPrimaryContainer 
                                        else MaterialTheme.colorScheme.onSurfaceVariant
                            )
                            Text(
                                text = "Chain: $linkedChainName",
                                fontSize = 13.sp,
                                color = if (isActive) MaterialTheme.colorScheme.onPrimaryContainer.copy(alpha = 0.75f)
                                        else Color.Gray,
                                modifier = Modifier.padding(top = 1.dp)
                            )
                        }

                        // Compact Action Buttons
                        Row(horizontalArrangement = Arrangement.spacedBy(2.dp)) {
                            IconButton(
                                onClick = {
                                    editingSongId = song.id
                                    inputTitle = song.title
                                    selectedChainName = linkedChainName
                                    showDialog = true
                                },
                                modifier = Modifier.size(36.dp)
                                ) {
                                Icon(Icons.Default.Edit, contentDescription = "Edit", tint = MaterialTheme.colorScheme.primary, modifier = Modifier.size(20.dp))
                            }
                            IconButton(
                                onClick = { onDeleteSong(song.id) },
                                modifier = Modifier.size(36.dp)
                            ) {
                                Icon(Icons.Default.Delete, contentDescription = "Delete", tint = Color.Red, modifier = Modifier.size(20.dp))
                            }
                        }
                    }
                }
            }
        }
    }

    // Modal Dialog for Adding / Editing Song
    if (showDialog) {
        AlertDialog(
            onDismissRequest = { showDialog = false },
            title = { 
                Text(
                    text = if (editingSongId == 0L) "Add Song" else "Edit Song",
                    fontWeight = FontWeight.Bold,
                    fontSize = 18.sp
                ) 
            },
            text = {
                Column(
                    modifier = Modifier
                        .fillMaxWidth(),
                    verticalArrangement = Arrangement.spacedBy(10.dp)
                ) {
                    OutlinedTextField(
                        value = inputTitle,
                        onValueChange = { inputTitle = it },
                        label = { Text("Song Title") },
                        modifier = Modifier.fillMaxWidth(),
                        singleLine = true
                    )

                    Box(modifier = Modifier.fillMaxWidth()) {
                        OutlinedButton(
                            onClick = { dropdownExpanded = true },
                            modifier = Modifier.fillMaxWidth(),
                            contentPadding = PaddingValues(10.dp)
                        ) {
                            Text(
                                text = if (selectedChainName.isBlank()) "Select Effect Chain" else "Chain: $selectedChainName",
                                fontSize = 15.sp
                            )
                        }
                        DropdownMenu(
                            expanded = dropdownExpanded,
                            onDismissRequest = { dropdownExpanded = false },
                            modifier = Modifier.fillMaxWidth(0.8f)
                        ) {
                            availableChains.forEach { chainName ->
                                DropdownMenuItem(
                                    text = { Text(chainName, fontSize = 15.sp) },
                                    onClick = {
                                        selectedChainName = chainName
                                        dropdownExpanded = false
                                    }
                                )
                            }
                        }
                    }
                }
            },
            confirmButton = {
                Button(
                    onClick = {
                        if (inputTitle.isNotBlank()) {
                            onSaveSong(editingSongId, inputTitle, selectedChainName)
                            showDialog = false
                        }
                    }
                ) {
                    Text("Save", fontWeight = FontWeight.Bold)
                }
            },
            dismissButton = {
                TextButton(onClick = { showDialog = false }) {
                    Text("Cancel")
                }
            }
        )
    }
}
