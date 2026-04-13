package com.example.myapplication.app

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.example.myapplication.bridge.CoreSnapshotMapper.toDskyState
import com.example.myapplication.core.CoreSnapshot
import com.example.myapplication.core.NativeApolloCore
import com.example.myapplication.data.program.ProgramAssetLoader
import com.example.myapplication.data.source.SourceAnnotationMapper
import com.example.myapplication.data.source.SourceAnnotationRepository
import com.example.myapplication.data.source.SourceAnnotationState
import com.example.myapplication.domain.mission.MissionCatalog
import com.example.myapplication.domain.mission.SoftwareVariant
import com.example.myapplication.dsky.model.DskyState
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

enum class AppScreen {
    TITLE,
    SELECTION,
    SIMULATOR,
    DEBRIEF,
    AUTHENTICITY,
    SOURCE_BROWSER,
}

data class AppUiState(
    val screen: AppScreen = AppScreen.TITLE,
    val sourceBrowserReturnScreen: AppScreen = AppScreen.TITLE,
    val selectedVariant: SoftwareVariant = MissionCatalog.defaultVariant(),
    val availableVariants: List<SoftwareVariant> = MissionCatalog.variants,
    val snapshot: CoreSnapshot = CoreSnapshot.initial(),
    val dskyState: DskyState = CoreSnapshot.initial().toDskyState(),
    val paused: Boolean = false,
    val sourceAnnotations: SourceAnnotationState = SourceAnnotationState(),
)

class ApolloSimViewModel(application: Application) : AndroidViewModel(application) {
    private val core = NativeApolloCore()
    private val programAssetLoader = ProgramAssetLoader(application.assets)
    private val sourceRepository = SourceAnnotationRepository(application.assets)
    private val _uiState = MutableStateFlow(
        AppUiState(
            selectedVariant = MissionCatalog.defaultVariant(),
            availableVariants = MissionCatalog.variants,
            sourceAnnotations = SourceAnnotationState(catalog = sourceRepository.load()),
        ),
    )
    val uiState: StateFlow<AppUiState> = _uiState

    init {
        core.initCore()
        viewModelScope.launch {
            while (true) {
                delay(100L)
                tick(0.1)
            }
        }
    }

    fun startScenario() {
        val selected = _uiState.value.selectedVariant
        core.initCore()
        val bytes = programAssetLoader.loadPackage(selected)
        core.loadProgramImage(bytes)
        val snapshot = core.getSnapshot()
        _uiState.value = AppUiState(
            screen = AppScreen.SIMULATOR,
            selectedVariant = selected,
            availableVariants = MissionCatalog.variants,
            snapshot = snapshot,
            dskyState = snapshot.toDskyState(),
            sourceAnnotations = buildSourceState(
                current = _uiState.value.sourceAnnotations,
                snapshot = snapshot,
            ),
        )
    }

    fun openAuthenticity() {
        _uiState.update { it.copy(screen = AppScreen.AUTHENTICITY) }
    }

    fun openSelection() {
        _uiState.update { it.copy(screen = AppScreen.SELECTION) }
    }

    fun openSourceBrowser() {
        _uiState.update { it.copy(screen = AppScreen.SOURCE_BROWSER, sourceBrowserReturnScreen = it.screen) }
    }

    fun returnToMenu() {
        _uiState.update { it.copy(screen = AppScreen.TITLE, paused = true) }
    }

    fun backToSimulator() {
        _uiState.update { it.copy(screen = it.sourceBrowserReturnScreen) }
    }

    fun adjustThrottle(delta: Double) {
        if (delta > 0.0) {
            core.pressKey("TRIM_UP")
        } else {
            core.pressKey("TRIM_DOWN")
        }
        refreshFromCore()
    }

    fun togglePause() {
        _uiState.update { it.copy(paused = !it.paused) }
    }

    fun toggleEngineerMode() {
        _uiState.update {
            it.copy(
                sourceAnnotations = it.sourceAnnotations.copy(
                    engineerModeEnabled = !it.sourceAnnotations.engineerModeEnabled,
                ),
            )
        }
    }

    fun toggleSourcePanel() {
        _uiState.update {
            it.copy(
                sourceAnnotations = it.sourceAnnotations.copy(
                    sourcePanelExpanded = !it.sourceAnnotations.sourcePanelExpanded,
                ),
            )
        }
    }

    fun selectVariant(variantId: String) {
        val selected = _uiState.value.availableVariants.firstOrNull { it.id == variantId } ?: return
        _uiState.update { it.copy(selectedVariant = selected) }
    }

    fun onKeyLabel(label: String) {
        core.pressKey(label)
        refreshFromCore()
    }

    private fun tick(deltaSeconds: Double) {
        val current = _uiState.value
        if (current.screen != AppScreen.SIMULATOR || current.paused || current.snapshot.missionResult.isNotEmpty()) {
            return
        }
        core.stepSimulation(deltaSeconds)
        refreshFromCore()
    }

    private fun refreshFromCore() {
        val snapshot = core.getSnapshot()
        _uiState.update {
            it.copy(
                screen = if (snapshot.missionResult.isNotEmpty()) AppScreen.DEBRIEF else AppScreen.SIMULATOR,
                snapshot = snapshot,
                dskyState = snapshot.toDskyState(),
                sourceAnnotations = buildSourceState(
                    current = it.sourceAnnotations,
                    snapshot = snapshot,
                ),
            )
        }
    }

    private fun buildSourceState(
        current: SourceAnnotationState,
        snapshot: CoreSnapshot,
    ): SourceAnnotationState {
        return current.copy(
            currentSnapshotProgram = snapshot.phaseProgram,
            currentNote = SourceAnnotationMapper.map(snapshot, current.catalog),
        )
    }
}
