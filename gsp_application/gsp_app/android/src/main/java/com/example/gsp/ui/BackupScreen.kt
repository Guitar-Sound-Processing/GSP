package com.example.gsp.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Build
import androidx.compose.material.icons.filled.Edit
import androidx.compose.material.icons.filled.Share
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.gsp.model.Screen
import com.example.gsp.storage.GspBackupData
import com.example.gsp.storage.ImportMode

@Composable
fun BackupScreen(
    isConnected: Boolean,
    onExportFile: () -> Unit,
    onShareBackup: () -> Unit,
    onPickImportFile: () -> Unit,
    pendingImportData: GspBackupData?,
    importErrorMessage: String? = null,
    onConfirmImport: (GspBackupData, ImportMode) -> Unit,
    onCancelImport: () -> Unit,
    logText: String,
    onNavigate: (Screen) -> Unit
) {
    var selectedImportMode by remember(pendingImportData) { mutableStateOf<ImportMode?>(null) }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(12.dp)
            .verticalScroll(rememberScrollState())
    ) {
        // Header Row with LED
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .padding(bottom = 8.dp)
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
                text = "Backup & Data Management",
                style = MaterialTheme.typography.titleMedium.copy(
                    fontWeight = FontWeight.Bold
                ),
                modifier = Modifier.align(Alignment.Center)
            )
        }

        Spacer(modifier = Modifier.height(8.dp))

        // Card displaying JSON error if import failed
        if (importErrorMessage != null) {
            Card(
                modifier = Modifier.fillMaxWidth(),
                shape = RoundedCornerShape(8.dp),
                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.errorContainer)
            ) {
                Column(modifier = Modifier.padding(12.dp)) {
                    Text(
                        text = "⚠️ JSON Import Error Detected",
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.onErrorContainer,
                        fontSize = 15.sp
                    )
                    Spacer(modifier = Modifier.height(4.dp))
                    Text(
                        text = importErrorMessage,
                        fontSize = 13.sp,
                        color = MaterialTheme.colorScheme.onErrorContainer
                    )
                }
            }
            Spacer(modifier = Modifier.height(10.dp))
        }

        if (pendingImportData == null) {
            // Card 1: Export & Share Section
            Card(
                modifier = Modifier.fillMaxWidth(),
                shape = RoundedCornerShape(10.dp),
                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant)
            ) {
                Column(
                    modifier = Modifier.padding(16.dp),
                    verticalArrangement = Arrangement.spacedBy(10.dp)
                ) {
                    Text(
                        text = "📤 Export & Backup",
                        fontWeight = FontWeight.Bold,
                        fontSize = 17.sp,
                        color = MaterialTheme.colorScheme.primary
                    )
                    Text(
                        text = "Generate a unified JSON file containing all your Songs, Setlists/Playlists, Rigs, and Presets to store in cloud storage or share.",
                        fontSize = 14.sp,
                        style = MaterialTheme.typography.bodyMedium
                    )

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        Button(
                            onClick = onExportFile,
                            modifier = Modifier
                                .weight(1f)
                                .height(48.dp),
                            shape = RoundedCornerShape(8.dp)
                        ) {
                            Icon(Icons.Default.Edit, contentDescription = null, modifier = Modifier.size(18.dp))
                            Spacer(modifier = Modifier.width(6.dp))
                            Text("Save File", fontWeight = FontWeight.Bold)
                        }

                        OutlinedButton(
                            onClick = onShareBackup,
                            modifier = Modifier
                                .weight(1f)
                                .height(48.dp),
                            shape = RoundedCornerShape(8.dp)
                        ) {
                            Icon(Icons.Default.Share, contentDescription = null, modifier = Modifier.size(18.dp))
                            Spacer(modifier = Modifier.width(6.dp))
                            Text("Share", fontWeight = FontWeight.Bold)
                        }
                    }
                }
            }

            Spacer(modifier = Modifier.height(14.dp))

            // Card 2: Import Section
            Card(
                modifier = Modifier.fillMaxWidth(),
                shape = RoundedCornerShape(10.dp),
                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant)
            ) {
                Column(
                    modifier = Modifier.padding(16.dp),
                    verticalArrangement = Arrangement.spacedBy(10.dp)
                ) {
                    Text(
                        text = "📥 File Import",
                        fontWeight = FontWeight.Bold,
                        fontSize = 17.sp,
                        color = MaterialTheme.colorScheme.primary
                    )
                    Text(
                        text = "Load a .json backup file from your device or cloud storage (Google Drive, WhatsApp, Downloads).",
                        fontSize = 14.sp,
                        style = MaterialTheme.typography.bodyMedium
                    )

                    Button(
                        onClick = onPickImportFile,
                        modifier = Modifier
                            .fillMaxWidth()
                            .height(48.dp),
                        colors = ButtonDefaults.buttonColors(containerColor = MaterialTheme.colorScheme.secondary),
                        shape = RoundedCornerShape(8.dp)
                    ) {
                        Icon(Icons.Default.Build, contentDescription = null, modifier = Modifier.size(20.dp))
                        Spacer(modifier = Modifier.width(8.dp))
                        Text("Select JSON File", fontWeight = FontWeight.Bold)
                    }
                }
            }
        } else {
            // Card 3: Import Mode Confirmation Section (Mandatory Radio Button Choice)
            Card(
                modifier = Modifier.fillMaxWidth(),
                shape = RoundedCornerShape(10.dp),
                colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.primaryContainer)
            ) {
                Column(
                    modifier = Modifier.padding(16.dp),
                    verticalArrangement = Arrangement.spacedBy(12.dp)
                ) {
                    Text(
                        text = "📥 Confirm Backup Import",
                        fontWeight = FontWeight.Bold,
                        fontSize = 18.sp,
                        color = MaterialTheme.colorScheme.onPrimaryContainer
                    )

                    Text(
                        text = "Backup file loaded successfully (${pendingImportData.timestamp}).\n\nContent found:\n• ${pendingImportData.songs.size} Songs\n• ${pendingImportData.playlists.size} Playlists\n• ${pendingImportData.chains.size} Rigs/Chains\n• ${pendingImportData.presets.size} Effects with Presets\n\nPlease select an import mode below to proceed:",
                        fontSize = 14.sp,
                        style = MaterialTheme.typography.bodyMedium,
                        color = MaterialTheme.colorScheme.onPrimaryContainer
                    )

                    HorizontalDivider(modifier = Modifier.padding(vertical = 4.dp))

                    // Radio Button Option 1: Merge
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        modifier = Modifier
                            .fillMaxWidth()
                            .clickable { selectedImportMode = ImportMode.MERGE }
                            .padding(vertical = 6.dp)
                    ) {
                        RadioButton(
                            selected = selectedImportMode == ImportMode.MERGE,
                            onClick = { selectedImportMode = ImportMode.MERGE }
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Column {
                            Text("Lossless Append (Merge)", fontWeight = FontWeight.Bold, fontSize = 15.sp)
                            Text("Preserves your current database and appends new backup items.", fontSize = 12.sp, color = MaterialTheme.colorScheme.onSurfaceVariant)
                        }
                    }

                    // Radio Button Option 2: Overwrite
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        modifier = Modifier
                            .fillMaxWidth()
                            .clickable { selectedImportMode = ImportMode.OVERWRITE }
                            .padding(vertical = 6.dp)
                    ) {
                        RadioButton(
                            selected = selectedImportMode == ImportMode.OVERWRITE,
                            onClick = { selectedImportMode = ImportMode.OVERWRITE }
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Column {
                            Text("Overwrite (Replace All)", fontWeight = FontWeight.Bold, fontSize = 15.sp, color = MaterialTheme.colorScheme.error)
                            Text("Deletes all current data and restores from the backup file.", fontSize = 12.sp, color = MaterialTheme.colorScheme.onSurfaceVariant)
                        }
                    }

                    Spacer(modifier = Modifier.height(8.dp))

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        OutlinedButton(
                            onClick = onCancelImport,
                            modifier = Modifier
                                .weight(1f)
                                .height(48.dp),
                            shape = RoundedCornerShape(8.dp)
                        ) {
                            Text("Cancel", fontWeight = FontWeight.Bold)
                        }

                        Button(
                            onClick = {
                                selectedImportMode?.let { mode ->
                                    onConfirmImport(pendingImportData, mode)
                                }
                            },
                            enabled = selectedImportMode != null, // 🔒 Mandatory selection
                            colors = ButtonDefaults.buttonColors(
                                containerColor = if (selectedImportMode == ImportMode.OVERWRITE) MaterialTheme.colorScheme.error else MaterialTheme.colorScheme.primary
                            ),
                            modifier = Modifier
                                .weight(1.5f)
                                .height(48.dp),
                            shape = RoundedCornerShape(8.dp)
                        ) {
                            Text("Confirm Import", fontWeight = FontWeight.Bold)
                        }
                    }
                }
            }
        }
    }
}
