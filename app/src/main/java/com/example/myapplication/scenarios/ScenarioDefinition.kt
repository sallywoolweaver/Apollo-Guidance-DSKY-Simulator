package com.example.myapplication.scenarios

import com.example.myapplication.domain.RealismProfile
import com.example.myapplication.simulation.flight.FlightState

data class ScenarioDefinition(
    val id: String,
    val title: String,
    val description: String,
    val authenticityNote: String,
    val initialFlightState: FlightState,
    val defaultRealismProfile: RealismProfile,
)
