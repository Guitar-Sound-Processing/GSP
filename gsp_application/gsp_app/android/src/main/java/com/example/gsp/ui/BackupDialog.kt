package com.example.gsp.ui

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.gsp.storage.GspBackupData
import com.example.gsp.storage.ImportMode

@Composable
fun BackupDialog(
    onDismiss: () -> Unit,
    onExportFile: () -> Unit,
    onShareBackup: () -> Unit,
    onPickImportFile: () -> Unit,
    pendingImportData: GspBackupData?,
    onConfirmImport: (GspBackupData, ImportMode) -> Unit,
    onCancelImport: () -> Unit
) {
    var selectedImportMode by remember(pendingImportData) { mutableStateOf<ImportMode?>(null) }

    AlertDialog(
        onDismissRequest = onDismiss,
        title = {
            Text(
                text = "Backup & Data Management",
                fontWeight = FontWeight.Bold,
                fontSize = 18.sp
            )
        },
        text = {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 4.dp),
                verticalArrangement = Arrangement.spacedBy(12.dp)
            ) {
                if (pendingImportData == null) {
                    Text(
                        text = "Data Export",
                        fontWeight = FontWeight.Bold,
                        fontSize = 15.sp,
                        color = MaterialTheme.colorScheme.primary
                    )

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        OutlinedButton(
                            onClick = onExportFile,
                            modifier = Modifier.weight(1f),
                            shape = RoundedCornerShape(6.dp)
                        ) {
                            Text("Save File", fontSize = 13.sp, fontWeight = FontWeight.Bold)
                        }

                        OutlinedButton(
                            onClick = onShareBackup,
                            modifier = Modifier.weight(1f),
                            shape = RoundedCornerShape(6.dp)
                        ) {
                            Text("Share", fontSize = 13.sp, fontWeight = FontWeight.Bold)
                        }
                    }

                    HorizontalDivider(modifier = Modifier.padding(vertical = 4.dp))

                    Text(
                        text = "Data Import",
                        fontWeight = FontWeight.Bold,
                        fontSize = 15.sp,
                        color = MaterialTheme.colorScheme.primary
                    )

                    Button(
                        onClick = onPickImportFile,
                        modifier = Modifier.fillMaxWidth(),
                        shape = RoundedCornerShape(6.dp)
                    ) {
                        Text("Select JSON File", fontSize = 14.sp, fontWeight = FontWeight.Bold)
                    }
                } else {
                    Text(
                        text = "Backup File Loaded!",
                        fontWeight = FontWeight.Bold,
                        fontSize = 16.sp,
                        color = MaterialTheme.colorScheme.primary
                    )

                    Text(
                        text = "Found ${pendingImportData.songs.size} songs, ${pendingImportData.playlists.size} playlists, and ${pendingImportData.chains.size} chains.\n\nPlease select an import mode below to proceed:",
                        fontSize = 13.sp,
                        style = MaterialTheme.typography.bodyMedium
                    )

                    Column(
                        modifier = Modifier.fillMaxWidth(),
                        verticalArrangement = Arrangement.spacedBy(4.dp)
                    ) {
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            modifier = Modifier
                                .fillMaxWidth()
                                .clickable { selectedImportMode = ImportMode.MERGE }
                                .padding(vertical = 4.dp)
                        ) {
                            RadioButton(
                                selected = selectedImportMode == ImportMode.MERGE,
                                onClick = { selectedImportMode = ImportMode.MERGE }
                            )
                            Spacer(modifier = Modifier.width(6.dp))
                            Column {
                                Text("Lossless Append (Merge)", fontWeight = FontWeight.Bold, fontSize = 14.sp)
                                Text("Appends new items without deleting existing data.", fontSize = 12.sp, color = MaterialTheme.colorScheme.onSurfaceVariant)
                            }
                        }

                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            modifier = Modifier
                                .fillMaxWidth()
                                .clickable { selectedImportMode = ImportMode.OVERWRITE }
                                .padding(vertical = 4.dp)
                        ) {
                            RadioButton(
                                selected = selectedImportMode == ImportMode.OVERWRITE,
                                onClick = { selectedImportMode = ImportMode.OVERWRITE }
                            )
                            Spacer(modifier = Modifier.width(6.dp))
                            Column {
                                Text("Overwrite (Replace All)", fontWeight = FontWeight.Bold, fontSize = 14.sp, color = MaterialTheme.colorScheme.error)
                                Text("Deletes all current data and restores from backup.", fontSize = 12.sp, color = MaterialTheme.colorScheme.onSurfaceVariant)
                            }
                        }
                    }
                }
            }
        },
        confirmButton = {
            if (pendingImportData != null) {
                Button(
                    onClick = {
                        selectedImportMode?.let { mode ->
                            onConfirmImport(pendingImportData, mode)
                        }
                    },
                    enabled = selectedImportMode != null,
                    colors = ButtonDefaults.buttonColors(
                        containerColor = if (selectedImportMode == ImportMode.OVERWRITE) MaterialTheme.colorScheme.error else MaterialTheme.colorScheme.primary
                    )
                ) {
                    Text("Confirm Import", fontWeight = FontWeight.Bold)
                }
            }
        },
        dismissButton = {
            TextButton(
                onClick = {
                    if (pendingImportData != null) {
                        onCancelImport()
                    } else {
                        onDismiss()
                    }
                }
            ) {
                Text(if (pendingImportData != null) "Cancel" else "Close")
            }
        }
    )
}
