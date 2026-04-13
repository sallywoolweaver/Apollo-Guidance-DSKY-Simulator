package com.example.myapplication

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.lifecycle.viewmodel.compose.viewModel
import com.example.myapplication.app.ApolloSimViewModel
import com.example.myapplication.app.AppScreen
import com.example.myapplication.ui.screens.AuthenticityScreen
import com.example.myapplication.ui.screens.DebriefScreen
import com.example.myapplication.ui.screens.SelectionScreen
import com.example.myapplication.ui.screens.SourceBrowserScreen
import com.example.myapplication.ui.screens.SimulatorScreen
import com.example.myapplication.ui.screens.TitleScreen
import com.example.myapplication.ui.theme.ApolloTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            ApolloTheme {
                val viewModel: ApolloSimViewModel = viewModel()
                val uiState by viewModel.uiState.collectAsState()
                when (uiState.screen) {
                    AppScreen.TITLE -> TitleScreen(
                        onStart = viewModel::startScenario,
                        onSelection = viewModel::openSelection,
                        onAuthenticity = viewModel::openAuthenticity,
                        onSourceBrowser = viewModel::openSourceBrowser,
                    )
                    AppScreen.SELECTION -> SelectionScreen(
                        uiState = uiState,
                        onSelectVariant = viewModel::selectVariant,
                        onLaunchSelected = viewModel::startScenario,
                        onBack = viewModel::returnToMenu,
                    )
                    AppScreen.SIMULATOR -> SimulatorScreen(
                        uiState = uiState,
                        onKey = viewModel::onKeyLabel,
                        onThrottleDown = { viewModel.adjustThrottle(-0.08) },
                        onThrottleUp = { viewModel.adjustThrottle(0.08) },
                        onPauseToggle = viewModel::togglePause,
                        onEngineerModeToggle = viewModel::toggleEngineerMode,
                        onSourcePanelToggle = viewModel::toggleSourcePanel,
                        onSourceBrowser = viewModel::openSourceBrowser,
                        onMenu = viewModel::returnToMenu,
                    )
                    AppScreen.DEBRIEF -> DebriefScreen(
                        uiState = uiState,
                        onRestart = viewModel::startScenario,
                        onMenu = viewModel::returnToMenu,
                    )
                    AppScreen.AUTHENTICITY -> AuthenticityScreen(onBack = viewModel::returnToMenu)
                    AppScreen.SOURCE_BROWSER -> SourceBrowserScreen(
                        uiState = uiState,
                        onBack = viewModel::backToSimulator,
                    )
                }
            }
        }
    }
}
