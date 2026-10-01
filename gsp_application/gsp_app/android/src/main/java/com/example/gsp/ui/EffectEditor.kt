package com.example.gsp.ui

import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.foundation.layout.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import com.example.gsp.model.Effect
import com.example.gsp.model.EffectParameter
import com.example.gsp.model.LfoParameter
import com.example.gsp.storage.EffectPreset
import kotlin.math.roundToInt

@Composable
fun EffectEditor(
    effect: Effect,
    onParamChange: (Int, EffectParameter, Boolean) -> Unit,
    onToggle: () -> Unit,
    onAddPreset: (String) -> Unit,
    onSavePreset: (String) -> Unit,
    onLoadPreset: (String) -> Unit,
    onDeletePreset: (String) -> Unit,
    onPresetNameChange: (String) -> Unit,
    presetName: String,
    presets: List<EffectPreset>
) {

    //var showDialog by remember { mutableStateOf(false) }
    //var tempName by remember { mutableStateOf("") }
    var expanded by remember { mutableStateOf(false) }
    var expandedLoad by remember { mutableStateOf(false) }
    var expandedDelete by remember { mutableStateOf(false) }
    var showAddDialog by remember { mutableStateOf(false) }

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 0.dp, vertical = 8.dp)
    ) {

        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween
        ) {

            Row(verticalAlignment = androidx.compose.ui.Alignment.CenterVertically) {

                if (effect.change) {
                    Text(
                        text = if (effect.enabled) "On" else "Bypass",
                        modifier = Modifier.width(70.dp),
                        textAlign = TextAlign.End
                    )

                    Spacer(modifier = Modifier.width(2.dp))

                    Switch(
                        checked = effect.enabled,
                        onCheckedChange = { onToggle() }
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        effect.parameters.forEachIndexed { index, param ->
            key(index) {

                Column {
                    // >>>>>>>>>>>>>>>>>>>>>> INT
                    val displayValue = when (param.type) {
                        EffectParameter.ParamType.INT ->
                            param.min + param.value * (param.max - param.min)
                        else ->
                            param.toSliderValue()
                    }
                    val valueText = when (param.type) {
                        EffectParameter.ParamType.FLOAT ->
                            "${"%.2f".format(displayValue)} ${param.unit}"
                        EffectParameter.ParamType.INT ->
                            "${displayValue.roundToInt()} ${param.unit}"
                        else -> ""
                    }
                    Text("${param.name}: $valueText")
                    Spacer(modifier = Modifier.height(2.dp))
                    if (param.type == EffectParameter.ParamType.INT) {
                        var expanded by remember { mutableStateOf(false)}
                        val options = (param.low.toInt()..param.high.toInt()).toList()
                        Box {
                            Button(
                                onClick = { expanded = true },
                                modifier = Modifier.fillMaxWidth()
                            ) {
                                Text("${displayValue.roundToInt()} ${param.unit}")
                            }
                            DropdownMenu(
                                expanded = expanded,
                                onDismissRequest = { expanded = false }
                            ) {
                                options.forEach { option ->
                                    DropdownMenuItem(
                                        text = { Text("$option ${param.unit}") },
                                        onClick = {
                                            expanded = false
                                            val normalized =
                                                (option - param.min) / (param.max - param.min)
                                            val updated = param.copy(
                                                value = normalized
                                            )
                                            onParamChange(index, updated, true) // ✅ correto
                                        }
                                    )
                                }
                            }
                        }
                    }

                    // >>>>>>>>>>>>>>>>>>>>>> FLOAT
                    if (param.type == EffectParameter.ParamType.FLOAT) {
                        var localValue by remember(param.value) {
                            mutableStateOf(param.value)
                        }
                        var sliderValue by remember { mutableStateOf(param.value) }
                        Slider(
                            value = param.value, // ✅ sempre 0..1
                            onValueChange = { newValue ->
                                onParamChange(index, param.copy(value = newValue), false)
                            },
                            onValueChangeFinished = {
                                onParamChange(index, param.copy(value = param.value), true)
                            },
                            valueRange = 0f..1f
                        )
                    }

                    // >>>>>>>>>>>>>>>>>>>>>> LFO
                    //if (param.type == EffectParameter.ParamType.LFO && param.lfo != null) {
                    if (param.type == EffectParameter.ParamType.LFO && param.lfo != null) {
                        val lfo = param.lfo!!
                        Column {
                            //Text("Profile")
                            // 🔽 WAVEFORM (Dropdown)
                            val waveOptions = LfoParameter.waveforms
                            Row(
                                modifier = Modifier.fillMaxWidth(),
                                horizontalArrangement = Arrangement.spacedBy(8.dp)
                            ) {
                                // PROFILE
                                Box(
                                    modifier = Modifier.weight(1f)
                                ) {
                                    Button(
                                        onClick = { expanded = true },
                                        modifier = Modifier.fillMaxWidth()
                                    ) {
                                        // Text inside button
                                        Text(lfo.waveformName())
                                    }
                                    DropdownMenu(
                                        expanded = expanded,
                                        onDismissRequest = { expanded = false }
                                    ) {
                                        waveOptions.forEachIndexed { i, label ->
                                            DropdownMenuItem(
                                                text = { Text(label) },
                                                onClick = {
                                                    expanded = false
                                                    val updated = param.copy(
                                                        lfo = lfo.copy(
                                                            waveform = i
                                                        )
                                                    )
                                                    onParamChange(index, updated, true)
                                                }
                                            )
                                        }
                                    }
                                }

                                // EXP PEDAL
                                val expEnabled =
                                    lfo.waveformName() == "Express Pedal"
                                var pedalExpanded by remember {
                                    mutableStateOf(false)
                                }
                                Box(
                                    modifier = Modifier.weight(1f)
                                ) {
                                    Button(
                                        onClick = {
                                            if (expEnabled)
                                                pedalExpanded = true
                                        },
                                        enabled = expEnabled,
                                        modifier = Modifier.fillMaxWidth()
                                    ) {
                                        val label =
                                            if (lfo.pedalChannel < 0)
                                                "Ext Pedal"
                                            else
                                                "Channel ${lfo.pedalChannel}"
                                        Text(label)
                                    }
                                    DropdownMenu(
                                        expanded = pedalExpanded,
                                        onDismissRequest = {
                                            pedalExpanded = false
                                        }
                                    ) {
                                        (0..7).forEach { ch ->
                                            DropdownMenuItem(
                                                text = { Text(ch.toString()) },
                                                onClick = {
                                                    println("PEDAL SELECTED = $ch")
                                                    pedalExpanded = false
                                                    val updated = param.copy(
                                                        lfo = lfo.copy(
                                                            pedalChannel = ch
                                                        )
                                                    )
                                                    println("pedalChannel = ${updated.lfo?.pedalChannel}")
                                                    onParamChange(index, updated, true)
                                                }
                                            )
                                        }
                                    }
                                }
                            }

                            Spacer(modifier = Modifier.height(8.dp))
                            val waveform = lfo.waveform
                            val showFrequency = LfoParameter.hasFrequency[waveform]
                            val showDuty = LfoParameter.hasDuty[waveform]

                            // 🎚️ FREQUENCY
                            if(showFrequency) {
                                Text("Frequency: ${"%.2f".format(lfo.frequency.toSliderValue())} Hz")
                                //Text("Frequency: ${"%.2f".format(lfo.frequency)} Hz")
                                Slider(
                                    value = lfo.frequency.value,
                                    onValueChange = { newValue ->
                                        val updated = param.copy(
                                            lfo = LfoParameter(
                                                waveform = lfo.waveform,
                                                frequency = lfo.frequency.copy(value = newValue),
                                                dutyCycle = lfo.dutyCycle,
                                                pedalChannel = lfo.pedalChannel
                                            )
                                        )
                                        onParamChange(index, updated, false)
                                    },
                                    onValueChangeFinished = {
                                        val updated = param.copy(
                                            lfo = LfoParameter(
                                                waveform = lfo.waveform,
                                                frequency = lfo.frequency,
                                                dutyCycle = lfo.dutyCycle,
                                                pedalChannel = lfo.pedalChannel
                                            )
                                        )
                                        onParamChange(index, updated, true)
                                    },
                                    valueRange = 0f..1f
                                )
                            }
                            // 🎚️ DUTY
                            if(showDuty) {
                                Text("Duty: ${(lfo.dutyCycle.value * 100).toInt()} %")
                                Slider(
                                    value = lfo.dutyCycle.value,
                                    onValueChange = { newValue ->
                                        val updated = param.copy(
                                            lfo = LfoParameter(
                                                waveform = lfo.waveform,
                                                frequency = lfo.frequency,
                                                dutyCycle = lfo.dutyCycle.copy(value = newValue),
                                                pedalChannel = lfo.pedalChannel
                                            )
                                        )
                                        onParamChange(index, updated, false)
                                    },
                                    onValueChangeFinished = {
                                        val updated = param.copy(
                                            lfo = LfoParameter(
                                                waveform = lfo.waveform,
                                                frequency = lfo.frequency,
                                                dutyCycle = lfo.dutyCycle,
                                                pedalChannel = lfo.pedalChannel
                                            )
                                        )
                                        onParamChange(index, updated, true)
                                    },
                                    valueRange = 0f..1f
                                )
                            }
                        }
                    }
                }
                Spacer(modifier = Modifier.height(8.dp))
            }
        }

        Spacer(modifier = Modifier.height(8.dp))

        Text(
            text = if (presetName.isBlank()) "No preset" else presetName,
            modifier = Modifier.fillMaxWidth(),
            textAlign = TextAlign.Center,
            style = MaterialTheme.typography.titleMedium,
            fontWeight = FontWeight.Bold
        )

        // Save, Load and Delete buttons
        Row(
            horizontalArrangement = Arrangement.SpaceEvenly,
            modifier = Modifier.fillMaxWidth()
        ) {

            Button(onClick = { showAddDialog = true }) {
                Text("Add")
            }

            //Button(onClick = { showDialog = true }) {
            //    Text("Save")
            //}

            Button(onClick = {
                if (presetName.isNotBlank()) {
                    onSavePreset(presetName)
                }
            }) {
                Text("Save")
            }

            Box {
                Button(onClick = { expandedLoad = true }) {
                    Text("Load")
                }

                DropdownMenu(
                    expanded = expandedLoad,
                    onDismissRequest = { expandedLoad = false }
                ) {
                    presets.forEach { preset ->

                        DropdownMenuItem(
                            text = { Text(preset.name) },
                            onClick = {
                                expandedLoad = false

                                onLoadPreset(preset.name)
                                onPresetNameChange(preset.name)
                            }
                        )
                    }
                }
            }

            Box {
                Button(onClick = { expandedDelete = true }) {
                    Text("Del")
                }

                DropdownMenu(
                    expanded = expandedDelete,
                    onDismissRequest = { expandedDelete = false }
                ) {

                    presets
                        .filter { it.name != presetName }
                        .forEach { preset ->

                            DropdownMenuItem(
                                text = { Text(preset.name) },
                                onClick = {
                                    expandedDelete = false
                                    onDeletePreset(preset.name)
                                }
                            )
                        }
                }
            }
            //Button(onClick = { onDeletePreset(presetName) }) {
            //    Text("Delete")
            //}
        }
    }

    if (showAddDialog) {

        var text by remember { mutableStateOf("") }

        AlertDialog(
            onDismissRequest = { showAddDialog = false },

            confirmButton = {
                Button(onClick = {
                    if (text.isNotBlank()) {
                        onAddPreset(text)
                        onPresetNameChange(text)
                    }
                    showAddDialog = false
                }) {
                    Text("Save")
                }
            },

            dismissButton = {
                Button(onClick = { showAddDialog = false }) {
                    Text("Cancel")
                }
            },

            text = {
                TextField(
                    value = text,
                    onValueChange = { text = it },
                    label = { Text("Preset name") }
                )
            }
        )
    }
}

