package com.example.myapplication.scenarios

import com.example.myapplication.simulation.flight.FlightState
import com.example.myapplication.simulation.mission.MissionPhase
import org.junit.Assert.assertEquals
import org.junit.Test

class ScenarioEngineTest {
    private val engine = ScenarioEngine()

    @Test
    fun `transitions from braking to approach`() {
        val updated = engine.update(
            previous = ScenarioState(Apollo11DescentScenario.definition(), phase = MissionPhase.BRAKING),
            flightState = FlightState(
                altitudeMeters = 800.0,
                verticalVelocityMps = -18.0,
                horizontalVelocityMps = 12.0,
                fuelKg = 500.0,
                throttle = 0.7,
                missionTimeSeconds = 60.0,
            ),
            alarmsOutstanding = false,
        )

        assertEquals(MissionPhase.APPROACH, updated.phase)
    }
}
