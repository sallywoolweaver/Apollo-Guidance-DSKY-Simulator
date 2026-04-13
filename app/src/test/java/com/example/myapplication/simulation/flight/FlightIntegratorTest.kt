package com.example.myapplication.simulation.flight

import com.example.myapplication.domain.RealismProfile
import com.example.myapplication.simulation.mission.MissionPhase
import org.junit.Assert.assertTrue
import org.junit.Test

class FlightIntegratorTest {
    @Test
    fun `step advances mission time and reduces altitude`() {
        val integrator = FlightIntegrator(RealismProfile.HISTORICAL_INSPIRED)
        val initial = FlightState(
            altitudeMeters = 2400.0,
            verticalVelocityMps = -30.0,
            horizontalVelocityMps = 20.0,
            fuelKg = 800.0,
            throttle = 0.5,
            missionTimeSeconds = 0.0,
        )

        val (next, _) = integrator.step(initial, MissionPhase.BRAKING, throttleBias = 0.0, deltaSeconds = 1.0)

        assertTrue(next.altitudeMeters < initial.altitudeMeters)
        assertTrue(next.fuelKg < initial.fuelKg)
        assertTrue(next.missionTimeSeconds > initial.missionTimeSeconds)
    }
}
