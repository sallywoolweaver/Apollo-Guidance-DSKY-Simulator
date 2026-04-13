package com.example.myapplication.simulation.flight

object LandingOutcomeClassifier {
    fun classify(flightState: FlightState): LandingOutcome {
        if (flightState.contact.not() && flightState.dry) {
            return LandingOutcome.FUEL_EXHAUSTED
        }

        val verticalImpact = kotlin.math.abs(flightState.verticalVelocityMps)
        val horizontalImpact = kotlin.math.abs(flightState.horizontalVelocityMps)

        return when {
            verticalImpact <= 3.0 && horizontalImpact <= 1.2 -> LandingOutcome.SAFE
            verticalImpact <= 6.0 && horizontalImpact <= 2.5 -> LandingOutcome.HARD
            else -> LandingOutcome.CRASH
        }
    }
}
