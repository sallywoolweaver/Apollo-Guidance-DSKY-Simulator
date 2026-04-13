package com.example.myapplication.ui.theme

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color

private val PanelColorScheme = darkColorScheme(
    primary = Color(0xFFE0A74A),
    onPrimary = Color(0xFF17120A),
    secondary = Color(0xFF7FA0A6),
    onSecondary = Color(0xFF101518),
    background = Color(0xFF080A0C),
    onBackground = Color(0xFFE8E2D2),
    surface = Color(0xFF13181D),
    onSurface = Color(0xFFE8E2D2),
    surfaceVariant = Color(0xFF20272D),
    onSurfaceVariant = Color(0xFFC1BAAA),
    error = Color(0xFFE9745B),
)

@Composable
fun ApolloTheme(
    content: @Composable () -> Unit,
) {
    MaterialTheme(
        colorScheme = PanelColorScheme,
        typography = MaterialTheme.typography,
        content = content,
    )
}
