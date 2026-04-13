package com.example.myapplication.scenarios

import com.example.myapplication.domain.RealismProfile
import com.example.myapplication.simulation.flight.FlightState

object Apollo11DescentScenario {
    fun definition(): ScenarioDefinition = ScenarioDefinition(
        id = "apollo11_descent_slice",
        title = "Apollo 11 Descent",
        description = "Apollo-inspired powered descent with DSKY monitoring, overload alarms, and a mobile-safe throttle trim concession.",
        authenticityNote = "Mission shape and alarm family are source-informed; initial conditions and exact event timing are approximated for this mobile slice.",
        initialFlightState = FlightState(
            altitudeMeters = 2_400.0,
            verticalVelocityMps = -34.0,
            horizontalVelocityMps = 24.0,
            fuelKg = 835.0,
            throttle = 0.62,
            missionTimeSeconds = 0.0,
        ),
        defaultRealismProfile = RealismProfile.HISTORICAL_INSPIRED,
    )
}
