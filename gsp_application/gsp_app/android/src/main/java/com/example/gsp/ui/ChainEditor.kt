package com.example.gsp.ui

import androidx.compose.runtime.*
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.*
import androidx.compose.ui.unit.sp
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.compose.ui.Alignment
import androidx.compose.ui.text.font.*
import androidx.compose.ui.text.style.*
import com.example.gsp.model.getAllEffects

@Composable
fun ChainEditor(
    chain: List<String>,
    chainName: String,
    currentIndex: Int,

    onScrollLeft: () -> Unit,
    onScrollRight: () -> Unit,
    onRemove: () -> Unit,
    onInsert: (String, Int) -> Unit,
    onNew: () -> Unit,
    onClear: () -> Unit,

    onAdd: (String) -> Unit,
    onSave: (String) -> Unit,
    onLoad: (String) -> Unit,
    onDelete: (String) -> Unit,
    onEditEffect: (String) -> Unit,

    getChains: () -> List<String>,
    refreshKey: Int,
    onChainNameChange: (String) -> Unit,
    logText: String,
    showDebug: Boolean = false
) {
    var showAddDialog by remember {
        mutableStateOf(false)
    }
    var tempChainName by remember {
        mutableStateOf("")
    }
    var debugText by remember {
        mutableStateOf("")
    }

    Column(modifier = Modifier.fillMaxSize().padding(12.dp)) {
        // New and Clear buttons
        Row(horizontalArrangement = Arrangement.SpaceEvenly, modifier = Modifier.fillMaxWidth()) {
            Button(onClick = onNew) { Text("New") }
            Button(onClick = onClear) { Text("Clear") }
        }
        // Effect scroll control
        val currentEffect = chain.getOrNull(currentIndex)
        Row(
            horizontalArrangement = Arrangement.SpaceEvenly,
            verticalAlignment = Alignment.CenterVertically,
            modifier = Modifier.fillMaxWidth()
        ) {
            // INSERT BEFORE
            InsertEffectDropdown(
                onInsert = onInsert,
                position = currentIndex
            )
            // REMOVE CURRENT
            Button(
                onClick = onRemove,
                enabled = currentEffect != null && currentEffect != "lvd"
            ) {
                Text(
                    "-",
                    style = MaterialTheme.typography.headlineMedium
                )
            }

            //println("CHAIN currentIndex = $currentIndex currentEffect = $currentEffect")

            // INSERT AFTER
            InsertEffectDropdown(
                onInsert = onInsert,
                position = currentIndex + 1
            )
        }

        // Effect box
        Row(
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceBetween,
            modifier = Modifier.fillMaxWidth()
        ) {
            Button(
                onClick = onScrollLeft,
                enabled = currentIndex > 0
            ) { Text("<") }
            EffectBox(
                id = currentEffect,
                onEditEffect = onEditEffect
            )
            Button(
                onClick = onScrollRight,
                enabled = currentIndex < chain.size - 1
            ) { Text(">") }
        }
        Text(
            text =
                if (chainName.isBlank())
                    "No Chain"
                else
                    chainName,
            modifier = Modifier.fillMaxWidth(),
            textAlign = TextAlign.Center,
            fontWeight = FontWeight.Bold
        )
        val chains = remember(refreshKey) {
            getChains()
        }
        Row(
            horizontalArrangement = Arrangement.SpaceEvenly,
            modifier = Modifier.fillMaxWidth()
        ) {
            Button(
                onClick = {
                    tempChainName = ""
                    showAddDialog = true
                }
            ) { Text("Add") }
            Button(
                onClick = {
                    if (chainName.isNotBlank()) {
                        onSave(chainName)
                        //debugText = chain.joinToString("\n")
                    }
                }
            ) { Text("Save") }
            ChainDropdown("Load", chains) {
                onLoad(it)
                onChainNameChange(it)
            }
            ChainDropdown("Del", chains) {
                onDelete(it)
            }
        }

        if (debugText.isNotBlank()) {
            Text(
                text = debugText,
                modifier = Modifier.fillMaxWidth()
            )
        }

        if (showDebug) {
            Spacer(modifier = Modifier.height(8.dp))
            Text(" Debug Log:")
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(120.dp)
                    .verticalScroll(rememberScrollState())
                    .padding(horizontal = 0.dp, vertical = 2.dp)
            ) {
                Text(logText, style = MaterialTheme.typography.bodySmall)
            }
        }

        // DEBUG ******************************************************
        if (showAddDialog) {
            AlertDialog(
                onDismissRequest = {
                    showAddDialog = false
                },
                title = { Text("New Chain") },
                text = {
                    OutlinedTextField(
                        value = tempChainName,
                        onValueChange = {
                            tempChainName = it
                        },
                        label = {
                            Text("Chain Name")
                        },
                        singleLine = true
                    )
                },
                confirmButton = {
                    Button(
                        onClick = {
                            if (tempChainName.isNotBlank()) {
                                onChainNameChange(
                                    tempChainName
                                )
                                onAdd(tempChainName)
                                showAddDialog = false
                            }
                        }
                    ) {
                        Text("Save")
                    }
                },
                dismissButton = {
                    Button(
                        onClick = {
                            showAddDialog = false
                        }
                    ) {
                        Text("Cancel")
                    }
                }
            )
        }
    }
}

// CURRENT EFFECT BOX ******************************************************
@Composable
fun EffectBox(
    id: String?,
    onEditEffect: (String) -> Unit
) {

    val effect =
        getAllEffects().firstOrNull { it.id == id }

    val label = when (id) {
        null -> "---"
        "lvd" -> "Level Detector"
        else -> effect?.name ?: id.uppercase()
    }

    Box(
        modifier = Modifier
            .size(120.dp, 80.dp)
            .padding(4.dp),
        contentAlignment = Alignment.Center
    ) {
        Card(
            modifier = Modifier.fillMaxSize(),
            onClick = {
                id?.let { onEditEffect(it) }
            }
        ) {
            Box(
                modifier = Modifier.fillMaxSize(),
                contentAlignment = Alignment.Center
            ) {
                Text(
                    text = label,
                    textAlign = TextAlign.Center,
                    modifier = Modifier.fillMaxWidth()
                )
            }
        }
    }
}

@Composable
fun InsertEffectDropdown(
    onInsert: (String, Int) -> Unit,
    position: Int
) {

    var expanded by remember { mutableStateOf(false) }

    val allEffects = remember { getAllEffects() }
    val allEffectsId =
        remember {
            allEffects
                .filter { it.id != "lvd" }
                .map { it.id }
        }

    Box {
        Button(onClick = { expanded = true }) {
            Text("+", style = MaterialTheme.typography.headlineMedium)
        }

        DropdownMenu(expanded = expanded, onDismissRequest = { expanded = false }) {

            allEffectsId.forEach { id ->
                DropdownMenuItem(
                    text = { Text(id.uppercase()) },
                    onClick = {
                        expanded = false
                        onInsert(id, position)
                    }
                )
            }
        }
    }
}

@Composable
fun ChainDropdown(
    label: String,
    items: List<String>,
    onSelect: (String) -> Unit
) {

    var expanded by remember { mutableStateOf(false) }

    Box {
        Button(onClick = { expanded = true }) {
            Text(label)
        }

        DropdownMenu(
            expanded = expanded,
            onDismissRequest = { expanded = false }
        ) {

            items.forEach { name ->
                DropdownMenuItem(
                    text = { Text(name) },
                    onClick = {
                        expanded = false
                        onSelect(name)
                    }
                )
            }
        }
    }
}

