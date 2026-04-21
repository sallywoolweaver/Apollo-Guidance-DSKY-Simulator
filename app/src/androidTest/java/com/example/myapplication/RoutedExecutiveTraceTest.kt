package com.example.myapplication

import android.util.Log
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.example.myapplication.core.NativeApolloCore
import com.example.myapplication.data.program.ProgramAssetLoader
import com.example.myapplication.domain.mission.MissionCatalog
import org.junit.Assert.assertTrue
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class RoutedExecutiveTraceTest {
    @Test
    fun tracesRoutedExecutiveKeyPath() {
        val context = InstrumentationRegistry.getInstrumentation().targetContext
        val core = NativeApolloCore()
        val loader = ProgramAssetLoader(context.assets)

        core.initCore()
        assertTrue(core.loadProgramImage(loader.loadPackage(MissionCatalog.defaultVariant())))

        repeat(30) {
            core.stepSimulation(0.1)
        }
        core.pressKey("1")
        repeat(10) {
            core.stepSimulation(0.1)
        }

        val snapshot = core.getSnapshot()
        Log.i(
            "ApolloExecTraceTest",
            "snapshot label=${snapshot.currentLabel} pc=${snapshot.programCounterBank}:${snapshot.programCounterOffset} note=${snapshot.executionNote}",
        )
    }
}
