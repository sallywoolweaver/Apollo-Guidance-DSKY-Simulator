package com.example.myapplication.simulation.flight

import org.junit.Assert.assertEquals
import org.junit.Test

class LandingOutcomeClassifierTest {
    @Test
    fun `classifies safe landing`() {
        val outcome = LandingOutcomeClassifier.classify(
            FlightState(
                altitudeMeters = 0.0,
                verticalVelocityMps = -2.4,
                horizontalVelocityMps = 0.9,
                fuelKg = 120.0,
                throttle = 0.0,
                missionTimeSeconds = 100.0,
                contact = true,
            ),
        )

        assertEquals(LandingOutcome.SAFE, outcome)
    }

    @Test
    fun `classifies crash`() {
        val outcome = LandingOutcomeClassifier.classify(
            FlightState(
                altitudeMeters = 0.0,
                verticalVelocityMps = -8.0,
                horizontalVelocityMps = 3.2,
                fuelKg = 80.0,
                throttle = 0.0,
                missionTimeSeconds = 100.0,
                contact = true,
            ),
        )

        assertEquals(LandingOutcome.CRASH, outcome)
    }
}
