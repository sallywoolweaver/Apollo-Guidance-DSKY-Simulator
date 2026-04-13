package com.example.myapplication.simulation.flight

data class FlightState(
    val altitudeMeters: Double,
    val verticalVelocityMps: Double,
    val horizontalVelocityMps: Double,
    val fuelKg: Double,
    val throttle: Double,
    val missionTimeSeconds: Double,
    val contact: Boolean = false,
) {
    val dry = fuelKg <= 0.0
}
