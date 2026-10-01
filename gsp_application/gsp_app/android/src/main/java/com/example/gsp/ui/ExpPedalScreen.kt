package com.example.gsp.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import com.example.gsp.model.ExpectedResponse
import com.example.gsp.model.Screen

@Composable
fun ExpPedalScreen(
    log: String,
    sendCommand: (String, ExpectedResponse) -> Unit,
    isConnected: Boolean,
    onNavigate: (Screen) -> Unit
) {

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 24.dp, vertical = 2.dp)
    ) {

        Box(
            modifier = Modifier
                .fillMaxWidth()
                .padding(horizontal = 16.dp)
        ) {
            // 🔴🟢 LED de status
            Box(
                modifier = Modifier
                    .size(16.dp)
                    .background(
                        color = if (isConnected) Color.Green else Color.Red,
                        shape = CircleShape
                    )
            )
            Text(
                text = "Song Editor",
                style = MaterialTheme.typography.titleLarge,
                modifier = Modifier.align(Alignment.Center)
            )
        }
    }
}
