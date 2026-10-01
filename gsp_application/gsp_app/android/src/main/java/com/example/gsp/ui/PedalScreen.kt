package com.example.gsp.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import com.example.gsp.model.Effect
import androidx.compose.ui.Alignment
import androidx.compose.ui.draw.scale
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.gsp.model.Screen
import com.example.gsp.storage.EffectPreset
import com.example.gsp.model.ExpectedResponse

@Composable
fun PedalScreen(
    effects: List<Effect>,
    currentIndex: Int,
    onNext: () -> Unit,
    onPrev: () -> Unit,
    onUpdateEffect: (Effect) -> Unit,
    onSendEffect: (Effect, Boolean, ExpectedResponse) -> Unit,
    onSendCommand: (String, ExpectedResponse) -> Unit,
    onAddPreset: (Effect, String) -> Unit,
    onSavePreset: (Effect, String) -> Unit,
    onLoadPreset: (Effect, String, ExpectedResponse) -> Unit,
    onDeletePreset: (Effect, String) -> Unit,
    getPresets: (String) -> List<EffectPreset>,
    logText: String,
    isConnected: Boolean,
    onNavigate: (Screen) -> Unit,
    showDebug: Boolean,
    onToggleDebug: () -> Unit
) {

    if (effects.isEmpty()) return

    val effect = effects[currentIndex]
    var presetName by remember(effect.id) { mutableStateOf("") }
    var refreshKey by remember { mutableStateOf(0) }

    LaunchedEffect(currentIndex) {
        onSendEffect(effect, false, ExpectedResponse.EFFECT) // 🔥 READ
    }

    LaunchedEffect(Unit) {
        onSendCommand("pot", ExpectedResponse.POT)
    }

//    val presets = remember(effect.id, refreshKey) {
//        getPresets(effect.id)
//    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(12.dp)
    ) {

        Box(
            modifier = Modifier
                .fillMaxWidth()
                .padding(bottom = 6.dp)
        ) {
            // 🔴🟢 LED de status (Esquerda)
            Box(
                modifier = Modifier
                    .size(16.dp)
                    .background(
                        color = if (isConnected) Color.Green else Color.Red,
                        shape = CircleShape
                    )
                    .align(Alignment.CenterStart)
            )

            // Título (Centro)
            Text(
                text = "Preset",
                style = MaterialTheme.typography.titleMedium.copy(
                    fontWeight = FontWeight.Bold
                ),
                modifier = Modifier.align(Alignment.Center)
            )

            // 🐛 Botão Debug (Direita)
            Row(
                verticalAlignment = Alignment.CenterVertically,
                modifier = Modifier.align(Alignment.CenterEnd)
            ) {
                Text(
                    text = "Debug",
                    fontSize = 12.sp,
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                    modifier = Modifier.padding(end = 2.dp)
                )
                Switch(
                    checked = showDebug,
                    onCheckedChange = { onToggleDebug() },
                    modifier = Modifier.scale(0.75f)
                )
            }
        }

        Row(
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceBetween,
            modifier = Modifier.fillMaxWidth()
        ) {
            Button(onClick = onPrev) { Text("<") }

            Text(effect.name)

            Button(onClick = onNext) { Text(">") }
        }

        Spacer(modifier = Modifier.height(16.dp))

        val presets = remember(effect.id, refreshKey) {
            getPresets(effect.id)
        }

        Column(
            modifier = Modifier
                .weight(1f)
                .verticalScroll(rememberScrollState())

        ) {
            EffectEditor(
                effect = effect,
                onParamChange = { index, updatedParam, shouldSend ->
                    val updated = effect.copy(
                        parameters = effect.parameters.mapIndexed { i, param ->
                            if (i == index) updatedParam else param
                        }
                    )
                    onUpdateEffect(updated)
                    if (shouldSend) {
                        onSendEffect(updated, true, ExpectedResponse.EFFECT)
                    }
                },
                onToggle = {
                    val updated = effect.copy(enabled = !effect.enabled)
                    onUpdateEffect(updated)
                    onSendEffect(updated, true, ExpectedResponse.EFFECT)
                },
                onAddPreset = { name ->
                    onAddPreset(effect, name)
                    refreshKey++
                },
                onSavePreset = { name ->
                    onSavePreset(effect, name)
                    refreshKey++
                },

                onLoadPreset = { name ->
                    onLoadPreset(effect, name, ExpectedResponse.EFFECT)
                    presetName = name
                },

                onDeletePreset = { name ->
                    onDeletePreset(effect, name)
                    refreshKey++
                },
                onPresetNameChange = { presetName = it },
                presetName = presetName,
                presets = presets
            )

            Spacer(modifier = Modifier.height(8.dp))

            if (showDebug) {
                Text(" Debug Log:")
                Column(
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(150.dp)
                        .verticalScroll(rememberScrollState())
                        .padding(horizontal = 0.dp, vertical = 2.dp)
                )
                {
                    //Text(cmdText,  style = MaterialTheme.typography.bodySmall)
                    Text(logText, style = MaterialTheme.typography.bodySmall)
                }
            }
        }
    }
}