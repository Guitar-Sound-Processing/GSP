package com.example.gsp.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Switch
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.scale
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.gsp.model.Effect
import com.example.gsp.model.ExpectedResponse

@Composable
fun ChainScreen(
    chain: List<String>,
    chainName: String,
    currentIndex: Int,
    onChainNameChange: (String) -> Unit,
    onCurrentIndexChange: (Int) -> Unit,
    effects: List<Effect>,
    sendCommand: (String, ExpectedResponse) -> Unit,
    getChains: () -> List<String>,
    loadChain: (String) -> List<String>?,
    saveChain: (String, List<String>) -> Unit,
    deleteChain: (String) -> Unit,
    onEditEffect: (String) -> Unit,
    logText: String,
    isConnected: Boolean,
    showDebug: Boolean,
    onToggleDebug: () -> Unit
) {

    var refreshKey by remember { mutableStateOf(0) }

    LaunchedEffect(Unit) {
        sendCommand("all", ExpectedResponse.CHAIN)
        sendCommand("pot", ExpectedResponse.POT)
    }

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
                text = "Chains",
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

        ChainEditor(
            chain = chain,
            chainName = chainName,
            currentIndex = currentIndex,
            onScrollLeft = {
                onCurrentIndexChange(maxOf(0, currentIndex - 1))
            },
            onScrollRight = {
                onCurrentIndexChange(minOf(chain.size - 1, currentIndex + 1))
            },
            onRemove = {
                val currentEffect = chain.getOrNull(currentIndex)
                currentEffect?.let {
                    sendCommand("$it (-1)", ExpectedResponse.EFFECT)
                    sendCommand("all", ExpectedResponse.CHAIN)
                }
            },
            onInsert = { id, insertPosition ->
                val effect = effects.firstOrNull { it.id == id }
                var pos = insertPosition

                if (
                    effect != null &&
                    effect.position >= 0 &&
                    effect.position < insertPosition
                ) {
                    pos--
                }
                pos--   // currentIndex - pos + 1 (starts with 1 instead of 0)

                sendCommand("$id ($pos)", ExpectedResponse.EFFECT)
                sendCommand("all", ExpectedResponse.CHAIN)
            },
            onNew = {
                sendCommand("new", ExpectedResponse.CHAIN)
                sendCommand("all", ExpectedResponse.CHAIN)
                onCurrentIndexChange(0)
                onChainNameChange("")
            },
            onClear = {
                sendCommand("clr", ExpectedResponse.CHAIN)
                sendCommand("all", ExpectedResponse.CHAIN)
                onCurrentIndexChange(0)
            },
            onAdd = { name ->
                saveChain(name, chain)
                onChainNameChange(name)
                refreshKey++
            },
            onSave = {
                saveChain(it, chain)
                refreshKey++
            },
            onLoad = {
                val loaded = loadChain(it)
                if (loaded != null) {
                    loaded.forEach { cmd ->
                        val expected = if (cmd.startsWith("pot")) {
                            ExpectedResponse.POT
                        } else {
                            ExpectedResponse.EFFECT
                        }
                        sendCommand(cmd, expected)
                    }
                    sendCommand("all", ExpectedResponse.CHAIN)
                    onChainNameChange(it)
                    onCurrentIndexChange(0)
                }
            },
            onDelete = {
                deleteChain(it)
                if (chainName == it) {
                    onChainNameChange("")
                    onCurrentIndexChange(0)
                }
                refreshKey++
            },
            getChains = {
                getChains()
            },
            refreshKey = refreshKey,
            onEditEffect = onEditEffect,
            onChainNameChange = { onChainNameChange(it) },
            logText = logText,
            showDebug = showDebug
        )
    }
}
