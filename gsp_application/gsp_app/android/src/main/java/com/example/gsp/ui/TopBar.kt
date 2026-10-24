package com.example.gsp.ui

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Share
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Shape
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.TextUnit
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.myapplication.R
import com.example.gsp.model.Screen

@Composable
fun TopBar(
    isConnected: Boolean,
    currentScreen: Screen,
    onPresets: () -> Unit,
    onChains: () -> Unit,
    onSongs: () -> Unit,
    onPlaylists: () -> Unit,
    onBackup: () -> Unit,
    // 🎨 Variáveis de Estilização Customizáveis dos Botões
    fontSize: TextUnit = 13.sp,
    fontWeight: FontWeight = FontWeight.Bold,
    buttonHeight: Dp = 42.dp,
    buttonShape: Shape = RoundedCornerShape(8.dp),
    iconSize: Dp = 32.dp, // 🔍 Tamanho ampliado dos ícones
    activeContainerColor: Color = MaterialTheme.colorScheme.primary,
    activeContentColor: Color = MaterialTheme.colorScheme.onPrimary,
    inactiveContainerColor: Color = MaterialTheme.colorScheme.surfaceVariant,
    inactiveContentColor: Color = MaterialTheme.colorScheme.onSurfaceVariant,
    elevation: Dp = 2.dp
) {
    val buttonElevation = ButtonDefaults.buttonElevation(defaultElevation = elevation)

    @Composable
    fun buttonColorsFor(screen: Screen): ButtonColors {
        return if (currentScreen == screen) {
            ButtonDefaults.buttonColors(
                containerColor = activeContainerColor,
                contentColor = activeContentColor
            )
        } else {
            ButtonDefaults.buttonColors(
                containerColor = inactiveContainerColor,
                contentColor = inactiveContentColor
            )
        }
    }

    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 4.dp, vertical = 4.dp),
        horizontalArrangement = Arrangement.spacedBy(3.dp)
    ) {
        val buttonModifier = Modifier
            .weight(1f)
            .height(buttonHeight)

        // 1. Playlists (i_playlists.png)
        Button(
            onClick = onPlaylists,
            modifier = buttonModifier,
            shape = buttonShape,
            colors = buttonColorsFor(Screen.PLAYLISTS),
            elevation = buttonElevation,
            contentPadding = PaddingValues(0.dp)
        ) {
            Icon(
                painter = painterResource(id = R.drawable.i_playlists),
                contentDescription = "Playlists",
                modifier = Modifier.size(iconSize),
                tint = Color.Unspecified
            )
        }

        // 2. Songs (i_songs.png)
        Button(
            onClick = onSongs,
            modifier = buttonModifier,
            shape = buttonShape,
            colors = buttonColorsFor(Screen.SONGS),
            elevation = buttonElevation,
            contentPadding = PaddingValues(0.dp)
        ) {
            Icon(
                painter = painterResource(id = R.drawable.i_songs),
                contentDescription = "Songs",
                modifier = Modifier.size(iconSize),
                tint = Color.Unspecified
            )
        }

        // 3. Chains / Rigs (i_chains.png)
        Button(
            onClick = onChains,
            modifier = buttonModifier,
            shape = buttonShape,
            colors = buttonColorsFor(Screen.CHAINS),
            elevation = buttonElevation,
            contentPadding = PaddingValues(0.dp)
        ) {
            Icon(
                painter = painterResource(id = R.drawable.i_chains),
                contentDescription = "Chains",
                modifier = Modifier.size(iconSize),
                tint = Color.Unspecified
            )
        }

        // 4. Presets (i_presets.png)
        Button(
            onClick = onPresets,
            modifier = buttonModifier,
            shape = buttonShape,
            colors = buttonColorsFor(Screen.PRESETS),
            elevation = buttonElevation,
            contentPadding = PaddingValues(0.dp)
        ) {
            Icon(
                painter = painterResource(id = R.drawable.i_presets),
                contentDescription = "Presets",
                modifier = Modifier.size(iconSize),
                tint = Color.Unspecified
            )
        }

        // 5. Backup / Restore / Import-Export
        Button(
            onClick = onBackup,
            modifier = buttonModifier,
            shape = buttonShape,
            colors = buttonColorsFor(Screen.BACKUP),
            elevation = buttonElevation,
            contentPadding = PaddingValues(0.dp)
        ) {
            Icon(
                imageVector = Icons.Default.Share,
                contentDescription = "Backup & Restore",
                modifier = Modifier.size(26.dp)
            )
        }
    }
}
