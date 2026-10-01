package com.example.gsp.ui

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Shape
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.TextUnit
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun TopBar(
    isConnected: Boolean,
    onPresets: () -> Unit,
    onChains: () -> Unit,
    onSongs: () -> Unit,
    onPlaylists: () -> Unit,
    // 🎨 Variáveis de Estilização Customizáveis dos Botões
    fontSize: TextUnit = 14.sp,
    fontWeight: FontWeight = FontWeight.Bold,
    buttonHeight: Dp = 40.dp,
    buttonShape: Shape = RoundedCornerShape(8.dp),
    containerColor: Color = MaterialTheme.colorScheme.primaryContainer,
    contentColor: Color = MaterialTheme.colorScheme.onPrimaryContainer,
    elevation: Dp = 2.dp
) {
    // Atribuição de propriedades unificadas para os botões
    val buttonColors = ButtonDefaults.buttonColors(
        containerColor = containerColor,
        contentColor = contentColor
    )
    val buttonElevation = ButtonDefaults.buttonElevation(defaultElevation = elevation)

    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 6.dp, vertical = 4.dp),
        horizontalArrangement = Arrangement.spacedBy(4.dp)
    ) {
        val buttonModifier = Modifier
            .weight(1f)
            .height(buttonHeight)

        Button(
            onClick = onPlaylists,
            modifier = buttonModifier,
            shape = buttonShape,
            colors = buttonColors,
            elevation = buttonElevation,
            contentPadding = PaddingValues(horizontal = 2.dp, vertical = 0.dp)
        ) {
            Text(
                text = "Play",
                fontSize = fontSize,
                fontWeight = fontWeight
            )
        }

        Button(
            onClick = onSongs,
            modifier = buttonModifier,
            shape = buttonShape,
            colors = buttonColors,
            elevation = buttonElevation,
            contentPadding = PaddingValues(horizontal = 2.dp, vertical = 0.dp)
        ) {
            Text(
                text = "Song",
                fontSize = fontSize,
                fontWeight = fontWeight
            )
        }

        Button(
            onClick = onChains,
            modifier = buttonModifier,
            shape = buttonShape,
            colors = buttonColors,
            elevation = buttonElevation,
            contentPadding = PaddingValues(horizontal = 2.dp, vertical = 0.dp)
        ) {
            Text(
                text = "Chain",
                fontSize = fontSize,
                fontWeight = fontWeight
            )
        }

        Button(
            onClick = onPresets,
            modifier = buttonModifier,
            shape = buttonShape,
            colors = buttonColors,
            elevation = buttonElevation,
            contentPadding = PaddingValues(horizontal = 2.dp, vertical = 0.dp)
        ) {
            Text(
                text = "Preset",
                fontSize = fontSize,
                fontWeight = fontWeight
            )
        }
    }
}
