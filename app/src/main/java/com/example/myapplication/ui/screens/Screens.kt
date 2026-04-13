package com.example.myapplication.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.widthIn
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Switch
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.myapplication.app.AppUiState
import com.example.myapplication.domain.mission.AuthenticityClassification
import com.example.myapplication.domain.mission.ScenarioSupportLevel
import com.example.myapplication.domain.mission.SoftwareVariant
import com.example.myapplication.data.source.SourceLabelInfo
import com.example.myapplication.data.source.SourceRelationTier
import com.example.myapplication.ui.components.DskyPanel
import com.example.myapplication.ui.components.InstrumentCard
import com.example.myapplication.ui.components.ThrottleTrim

@Composable
fun TitleScreen(
    onStart: () -> Unit,
    onSelection: () -> Unit,
    onAuthenticity: () -> Unit,
    onSourceBrowser: () -> Unit,
) {
    Scaffold { padding ->
        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(
                    Brush.verticalGradient(
                        colors = listOf(Color(0xFF080A0C), Color(0xFF12171B), Color(0xFF1C201B)),
                    ),
                )
                .padding(padding)
                .padding(24.dp),
        ) {
            Column(
                modifier = Modifier.widthIn(max = 720.dp),
                verticalArrangement = Arrangement.spacedBy(16.dp),
            ) {
                Text(
                    text = "Apollo Guidance DSKY Simulator",
                    color = MaterialTheme.colorScheme.onBackground,
                    fontSize = 34.sp,
                    fontWeight = FontWeight.Bold,
                    fontFamily = FontFamily.Monospace,
                )
                Text(
                    text = "Apollo 11 lunar module guidance computer / DSKY simulator",
                    color = MaterialTheme.colorScheme.primary,
                    fontSize = 18.sp,
                )
                Text(
                    text = "Vertical slice: one Apollo-inspired descent scenario, DSKY command entry, 1201/1202-style overload alarms, and a debrief grounded in explicit authenticity boundaries.",
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                    fontSize = 15.sp,
                )
                Button(onClick = onStart) {
                    Text("Begin Selected Scenario")
                }
                Button(onClick = onSelection) {
                    Text("Mission / Software Setup")
                }
                Button(onClick = onAuthenticity) {
                    Text("Authenticity and Sources")
                }
                Button(onClick = onSourceBrowser) {
                    Text("Source Browser")
                }
            }
        }
    }
}

@Composable
fun SelectionScreen(
    uiState: AppUiState,
    onSelectVariant: (String) -> Unit,
    onLaunchSelected: () -> Unit,
    onBack: () -> Unit,
) {
    Scaffold { padding ->
        LazyColumn(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF080A0C))
                .padding(padding),
            contentPadding = PaddingValues(24.dp),
            verticalArrangement = Arrangement.spacedBy(12.dp),
        ) {
            item {
                Text("Mission / Software Setup", color = MaterialTheme.colorScheme.onBackground, fontSize = 28.sp, fontWeight = FontWeight.Bold)
                Text(
                    "This screen separates software-family availability from actual execution support. Only the Apollo 11 LM slice is currently runnable.",
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                )
            }
            items(uiState.availableVariants) { variant ->
                val selected = variant.id == uiState.selectedVariant.id
                Card(
                    modifier = Modifier.clickable { onSelectVariant(variant.id) },
                    colors = CardDefaults.cardColors(
                        containerColor = if (selected) Color(0xFF1D262D) else MaterialTheme.colorScheme.surface,
                    ),
                ) {
                    Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                        Text(
                            "${variant.missionFamily.missionDisplayName} ${variant.vehicleType}",
                            color = MaterialTheme.colorScheme.primary,
                            fontWeight = FontWeight.Bold,
                        )
                        Text(
                            "${variant.softwareName} ${variant.buildNumber}",
                            color = MaterialTheme.colorScheme.onSurface,
                            fontWeight = FontWeight.SemiBold,
                        )
                        Text(variant.statusNote, color = MaterialTheme.colorScheme.onSurfaceVariant)
                        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                            StatusChip(variant.sourceStatus.displayName)
                            StatusChip(variant.scenarioSupportLevel.displayName)
                        }
                        AuthenticityChip(variant.authenticityClassification)
                    }
                }
            }
            item {
                Row(horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                    Button(
                        onClick = onLaunchSelected,
                        enabled = uiState.selectedVariant.scenarioSupportLevel == ScenarioSupportLevel.PLAYABLE,
                    ) {
                        Text("Launch Selected")
                    }
                    Button(onClick = onBack) { Text("Back") }
                }
            }
        }
    }
}

@Composable
fun SimulatorScreen(
    uiState: AppUiState,
    onKey: (String) -> Unit,
    onThrottleDown: () -> Unit,
    onThrottleUp: () -> Unit,
    onPauseToggle: () -> Unit,
    onEngineerModeToggle: () -> Unit,
    onSourcePanelToggle: () -> Unit,
    onSourceBrowser: () -> Unit,
    onMenu: () -> Unit,
) {
    Scaffold { padding ->
        LazyColumn(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF080A0C))
                .padding(padding),
            contentPadding = PaddingValues(16.dp),
            verticalArrangement = Arrangement.spacedBy(16.dp),
        ) {
            item {
                Text(
                    text = "Apollo 11 Descent",
                    color = MaterialTheme.colorScheme.onBackground,
                    fontSize = 24.sp,
                    fontWeight = FontWeight.Bold,
                )
                Text(
                    text = "Native emulator-oriented slice. DSKY command flow now lives in the native peripheral path, but descent, alarms, and outcomes still rely on a temporary compatibility layer rather than original Apollo execution.",
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                    fontSize = 13.sp,
                )
            }
            item {
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                    InstrumentCard("Phase", uiState.snapshot.phaseLabel, Modifier.weight(1f))
                    InstrumentCard("Load", "%.0f%%".format(uiState.snapshot.loadRatio * 100.0), Modifier.weight(1f))
                    InstrumentCard("Throttle", "%.0f%%".format(uiState.snapshot.throttle * 100.0), Modifier.weight(1f))
                }
            }
            item {
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                    InstrumentCard("Altitude", "%.0f m".format(uiState.snapshot.altitudeMeters), Modifier.weight(1f))
                    InstrumentCard("Vert Vel", "%.1f m/s".format(uiState.snapshot.verticalVelocityMps), Modifier.weight(1f))
                    InstrumentCard("Horiz Vel", "%.1f m/s".format(uiState.snapshot.horizontalVelocityMps), Modifier.weight(1f))
                    InstrumentCard("Fuel", "%.0f kg".format(uiState.snapshot.fuelKg), Modifier.weight(1f))
                }
            }
            item {
                ThrottleTrim(
                    onDown = onThrottleDown,
                    onUp = onThrottleUp,
                    value = uiState.snapshot.throttleTrim,
                )
            }
            item {
                DskyPanel(dskyState = uiState.dskyState, onKey = onKey)
            }
            item {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                ) {
                    Text(
                        text = "Engineer Mode",
                        color = MaterialTheme.colorScheme.onBackground,
                        fontWeight = FontWeight.SemiBold,
                    )
                    Switch(
                        checked = uiState.sourceAnnotations.engineerModeEnabled,
                        onCheckedChange = { onEngineerModeToggle() },
                    )
                }
            }
            if (uiState.sourceAnnotations.engineerModeEnabled) {
                item {
                    EngineerSourcePanel(
                        uiState = uiState,
                        onToggleExpanded = onSourcePanelToggle,
                        onOpenBrowser = onSourceBrowser,
                    )
                }
            }
            item {
                Row(horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                    Button(onClick = onPauseToggle) { Text(if (uiState.paused) "Resume" else "Pause") }
                    Button(onClick = onMenu) { Text("Return to Menu") }
                }
            }
        }
    }
}

@Composable
fun DebriefScreen(
    uiState: AppUiState,
    onRestart: () -> Unit,
    onMenu: () -> Unit,
) {
    val result = uiState.snapshot.missionResult
    if (result.isEmpty()) return
    Scaffold { padding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF080A0C))
                .padding(padding)
                .padding(24.dp),
            verticalArrangement = Arrangement.spacedBy(16.dp),
        ) {
            Text(text = result, color = MaterialTheme.colorScheme.onBackground, fontSize = 32.sp, fontWeight = FontWeight.Bold)
            Text(text = uiState.snapshot.missionResultSummary, color = MaterialTheme.colorScheme.onSurfaceVariant)
            Row(horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                InstrumentCard("Touchdown V", "%.1f m/s".format(uiState.snapshot.verticalVelocityMps), Modifier.weight(1f))
                InstrumentCard("Touchdown H", "%.1f m/s".format(uiState.snapshot.horizontalVelocityMps), Modifier.weight(1f))
                InstrumentCard("Fuel Left", "%.0f kg".format(uiState.snapshot.fuelKg), Modifier.weight(1f))
                InstrumentCard("Alarms", uiState.snapshot.totalAlarms.toString(), Modifier.weight(1f))
            }
            Card(colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surface)) {
                Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                    Text("Debrief", color = MaterialTheme.colorScheme.primary, fontWeight = FontWeight.Bold)
                    Text("Last phase event: ${uiState.snapshot.lastEvent}", color = MaterialTheme.colorScheme.onSurface)
                    Text("This outcome comes from the native core and companion descent model, not a full flown Luminary reproduction.", color = MaterialTheme.colorScheme.onSurfaceVariant)
                }
            }
            Row(horizontalArrangement = Arrangement.spacedBy(12.dp)) {
                Button(onClick = onRestart) { Text("Fly Again") }
                Button(onClick = onMenu) { Text("Main Menu") }
            }
        }
    }
}

@Composable
fun AuthenticityScreen(onBack: () -> Unit) {
    Scaffold { padding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF080A0C))
                .padding(padding)
                .padding(24.dp),
            verticalArrangement = Arrangement.spacedBy(16.dp),
        ) {
            Text("Authenticity Boundaries", color = MaterialTheme.colorScheme.onBackground, fontSize = 28.sp, fontWeight = FontWeight.Bold)
            Text("Exact source-derived: program numbers P63/P64/P66, DSKY verb/noun framing, and the 1201/1202 executive overflow alarm family.", color = MaterialTheme.colorScheme.onSurface)
            Text("Approximated: descent equations, alarm windows, throttle trim interaction, and mobile register layouts.", color = MaterialTheme.colorScheme.onSurface)
            Text("See docs in the project root for the full mapping, assumptions, and verification backlog.", color = MaterialTheme.colorScheme.onSurfaceVariant)
            Button(onClick = onBack) { Text("Back") }
        }
    }
}

@Composable
fun SourceBrowserScreen(
    uiState: AppUiState,
    onBack: () -> Unit,
) {
    val labels = uiState.sourceAnnotations.catalog.labels.values.sortedWith(
        compareBy<SourceLabelInfo>({ it.file }, { it.id }),
    )
    Scaffold { padding ->
        LazyColumn(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF080A0C))
                .padding(padding),
            contentPadding = PaddingValues(24.dp),
            verticalArrangement = Arrangement.spacedBy(12.dp),
        ) {
            item {
                Text("Source Browser", color = MaterialTheme.colorScheme.onBackground, fontSize = 28.sp, fontWeight = FontWeight.Bold)
                Text(
                    "Curated Luminary099 source annotations. These notes are authenticity overlays and are not automatic proof of live routine execution.",
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                )
            }
            items(labels) { label ->
                Card(colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surface)) {
                    Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                        Text(label.id, color = MaterialTheme.colorScheme.primary, fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                        Text(label.title, color = MaterialTheme.colorScheme.onSurface, fontWeight = FontWeight.SemiBold)
                        Text(label.file, color = MaterialTheme.colorScheme.onSurfaceVariant, fontSize = 12.sp)
                        Text(label.summary, color = MaterialTheme.colorScheme.onSurface)
                        Text("Original note", color = MaterialTheme.colorScheme.primary, fontSize = 12.sp)
                        Text(label.commentBlock, color = MaterialTheme.colorScheme.onSurfaceVariant, fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                    }
                }
            }
            item {
                Button(onClick = onBack) { Text("Back") }
            }
        }
    }
}

@Composable
private fun EngineerSourcePanel(
    uiState: AppUiState,
    onToggleExpanded: () -> Unit,
    onOpenBrowser: () -> Unit,
) {
    val currentNote = uiState.sourceAnnotations.currentNote
    Card(colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surface)) {
        Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
            Text("Engineer Source Panel", color = MaterialTheme.colorScheme.primary, fontWeight = FontWeight.Bold)
            if (currentNote == null) {
                Text(
                    "No source note is mapped for the current simulator state.",
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                )
            } else {
                SourceRelationChip(currentNote.relationTier)
                Text(currentNote.labelInfo.id, color = MaterialTheme.colorScheme.onSurface, fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold)
                Text(currentNote.labelInfo.file, color = MaterialTheme.colorScheme.onSurfaceVariant, fontSize = 12.sp)
                Text(currentNote.labelInfo.commentExcerpt, color = MaterialTheme.colorScheme.onSurface)
                Text(currentNote.modernContext, color = MaterialTheme.colorScheme.onSurfaceVariant, fontSize = 12.sp)
                Text(
                    if (uiState.sourceAnnotations.sourcePanelExpanded) "Hide full source note" else "Show full source note",
                    modifier = Modifier.clickable(onClick = onToggleExpanded),
                    color = MaterialTheme.colorScheme.primary,
                    fontSize = 12.sp,
                    fontWeight = FontWeight.SemiBold,
                )
                if (uiState.sourceAnnotations.sourcePanelExpanded) {
                    Text(
                        "Original source text",
                        color = MaterialTheme.colorScheme.primary,
                        fontSize = 12.sp,
                    )
                    Text(
                        currentNote.labelInfo.commentBlock,
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                        fontFamily = FontFamily.Monospace,
                        fontSize = 12.sp,
                    )
                }
            }
            Button(onClick = onOpenBrowser) { Text("Open Source Browser") }
        }
    }
}

@Composable
private fun SourceRelationChip(tier: SourceRelationTier) {
    Card(colors = CardDefaults.cardColors(containerColor = Color(0xFF23303A))) {
        Text(
            text = tier.displayName,
            modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp),
            color = Color(0xFFD3E7F6),
            fontSize = 11.sp,
            fontWeight = FontWeight.SemiBold,
        )
    }
}

@Composable
private fun StatusChip(label: String) {
    Card(colors = CardDefaults.cardColors(containerColor = Color(0xFF2B3238))) {
        Text(
            text = label,
            modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp),
            color = Color(0xFFDDE3E8),
            fontSize = 11.sp,
        )
    }
}

@Composable
private fun AuthenticityChip(classification: AuthenticityClassification) {
    val color = when (classification) {
        AuthenticityClassification.EXACT_SOURCE_BACKED_EXECUTION -> Color(0xFF244930)
        AuthenticityClassification.SOURCE_INFORMED_EXECUTION_WITH_COMPANION_TRANSLATION -> Color(0xFF3A3422)
        AuthenticityClassification.HISTORICAL_APPROXIMATION -> Color(0xFF4A2E21)
        AuthenticityClassification.PLANNED_PLACEHOLDER -> Color(0xFF2B2B36)
    }
    Card(colors = CardDefaults.cardColors(containerColor = color)) {
        Text(
            text = classification.displayName,
            modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp),
            color = Color(0xFFF2EBDD),
            fontSize = 11.sp,
            fontWeight = FontWeight.SemiBold,
        )
    }
}
