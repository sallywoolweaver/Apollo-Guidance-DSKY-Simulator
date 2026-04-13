package com.example.myapplication.ui.components

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.myapplication.dsky.model.Annunciator
import com.example.myapplication.dsky.model.DskyState

@Composable
fun InstrumentCard(
    title: String,
    value: String,
    modifier: Modifier = Modifier,
) {
    Card(
        modifier = modifier,
        colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surface),
    ) {
        Column(modifier = Modifier.padding(12.dp)) {
            Text(text = title, color = MaterialTheme.colorScheme.onSurfaceVariant, fontSize = 12.sp)
            Text(
                text = value,
                color = MaterialTheme.colorScheme.onSurface,
                fontSize = 24.sp,
                fontFamily = FontFamily.Monospace,
                fontWeight = FontWeight.Bold,
            )
        }
    }
}

@Composable
fun AnnunciatorRow(dskyState: DskyState) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.spacedBy(8.dp),
    ) {
        Annunciator.values().forEach { annunciator ->
            val active = dskyState.annunciators.contains(annunciator)
            Card(
                colors = CardDefaults.cardColors(
                    containerColor = if (active) Color(0xFF7A251B) else MaterialTheme.colorScheme.surfaceVariant,
                ),
            ) {
                Box(
                    modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp),
                    contentAlignment = Alignment.Center,
                ) {
                    Text(
                        text = annunciator.name.replace('_', ' '),
                        color = if (active) Color(0xFFFFD0BA) else MaterialTheme.colorScheme.onSurfaceVariant,
                        fontSize = 11.sp,
                        fontWeight = FontWeight.SemiBold,
                    )
                }
            }
        }
    }
}

@Composable
fun DskyPanel(
    dskyState: DskyState,
    onKey: (String) -> Unit,
    modifier: Modifier = Modifier,
) {
    Card(
        modifier = modifier,
        colors = CardDefaults.cardColors(containerColor = Color(0xFF0E1114)),
        shape = RoundedCornerShape(20.dp),
    ) {
        Column(
            modifier = Modifier.padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(12.dp),
        ) {
            AnnunciatorRow(dskyState)
            DisplayWindow(label = "PROG", value = dskyState.program)
            Row(horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                DisplayWindow(label = "VERB", value = dskyState.verb ?: "--", modifier = Modifier.weight(1f))
                DisplayWindow(label = "NOUN", value = dskyState.noun ?: "--", modifier = Modifier.weight(1f))
            }
            DisplayWindow(label = "R1", value = dskyState.register1)
            DisplayWindow(label = "R2", value = dskyState.register2)
            DisplayWindow(label = "R3", value = dskyState.register3)
            Text(
                text = dskyState.statusLine,
                color = MaterialTheme.colorScheme.onSurfaceVariant,
                fontSize = 12.sp,
            )
            val rows = listOf(
                listOf("VERB", "NOUN", "CLR"),
                listOf("7", "8", "9"),
                listOf("4", "5", "6"),
                listOf("1", "2", "3"),
                listOf("KEY REL", "0", "ENTR"),
                listOf("PRO", "RSET"),
            )
            rows.forEach { row ->
                Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    row.forEach { label ->
                        PanelKey(label = label, modifier = Modifier.weight(1f)) { onKey(label) }
                    }
                    if (row.size == 2) {
                        Box(modifier = Modifier.weight(1f))
                    }
                }
            }
        }
    }
}

@Composable
private fun DisplayWindow(label: String, value: String, modifier: Modifier = Modifier) {
    Column(modifier = modifier) {
        Text(text = label, color = MaterialTheme.colorScheme.onSurfaceVariant, fontSize = 11.sp)
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .background(Color(0xFF06100C), RoundedCornerShape(10.dp))
                .border(1.dp, Color(0xFF223226), RoundedCornerShape(10.dp))
                .padding(horizontal = 12.dp, vertical = 8.dp),
        ) {
            Text(
                text = value,
                color = Color(0xFF93FF9C),
                fontFamily = FontFamily.Monospace,
                fontWeight = FontWeight.Bold,
                fontSize = 28.sp,
            )
        }
    }
}

@Composable
private fun PanelKey(
    label: String,
    modifier: Modifier = Modifier,
    onClick: () -> Unit,
) {
    Button(
        onClick = onClick,
        modifier = modifier,
        shape = RoundedCornerShape(10.dp),
        colors = ButtonDefaults.buttonColors(
            containerColor = Color(0xFF2A3036),
            contentColor = MaterialTheme.colorScheme.onSurface,
        ),
    ) {
        Text(text = label, fontSize = 13.sp)
    }
}

@Composable
fun ThrottleTrim(onDown: () -> Unit, onUp: () -> Unit, value: Double) {
    Card(colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surface)) {
        Column(
            modifier = Modifier.padding(12.dp),
            verticalArrangement = Arrangement.spacedBy(8.dp),
        ) {
            Text(text = "Throttle Trim", color = MaterialTheme.colorScheme.onSurfaceVariant, fontSize = 12.sp)
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp), verticalAlignment = Alignment.CenterVertically) {
                PanelKey(label = "-", modifier = Modifier.width(72.dp), onClick = onDown)
                Text(
                    text = "%+.0f%%".format(value * 100.0),
                    color = MaterialTheme.colorScheme.onSurface,
                    fontFamily = FontFamily.Monospace,
                    fontSize = 20.sp,
                )
                PanelKey(label = "+", modifier = Modifier.width(72.dp), onClick = onUp)
            }
        }
    }
}
