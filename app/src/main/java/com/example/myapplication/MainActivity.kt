package com.example.myapplication

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.runtime.LaunchedEffect
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
    companion object {
        private const val TAG = "ApolloLaunch"
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.i(TAG, "MainActivity.onCreate savedInstanceState=${savedInstanceState != null}")
        enableEdgeToEdge()
        setContent {
            ApolloTheme {
                val viewModel: ApolloSimViewModel = viewModel()
                val uiState by viewModel.uiState.collectAsState()
                LaunchedEffect(uiState.screen) {
                    Log.i(TAG, "screen=${uiState.screen} paused=${uiState.paused} missionResult=${uiState.snapshot.missionResult.isNotEmpty()}")
                }
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

    override fun onStart() {
        super.onStart()
        Log.i(TAG, "MainActivity.onStart")
    }

    override fun onResume() {
        super.onResume()
        Log.i(TAG, "MainActivity.onResume")
    }

    override fun onPause() {
        Log.i(TAG, "MainActivity.onPause isFinishing=$isFinishing")
        super.onPause()
    }

    override fun onStop() {
        Log.i(TAG, "MainActivity.onStop isFinishing=$isFinishing")
        super.onStop()
    }

    override fun onDestroy() {
        Log.i(TAG, "MainActivity.onDestroy isFinishing=$isFinishing")
        super.onDestroy()
    }
}
