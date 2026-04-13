package com.example.myapplication.scenarios

import com.example.myapplication.simulation.flight.FlightState
import com.example.myapplication.simulation.flight.LandingOutcome
import com.example.myapplication.simulation.flight.LandingOutcomeClassifier
import com.example.myapplication.simulation.mission.MissionPhase

data class ScenarioState(
    val definition: ScenarioDefinition,
    val phase: MissionPhase = MissionPhase.BRAKING,
    val missionResult: LandingOutcome? = null,
    val lastEvent: String = "P63 initialized",
)

class ScenarioEngine {
    fun update(
        previous: ScenarioState,
        flightState: FlightState,
        alarmsOutstanding: Boolean,
    ): ScenarioState {
        if (previous.missionResult != null) {
            return previous
        }

        if (flightState.contact) {
            val result = LandingOutcomeClassifier.classify(flightState)
            return previous.copy(
                phase = if (result == LandingOutcome.CRASH) MissionPhase.FAILURE else MissionPhase.LANDED,
                missionResult = result,
                lastEvent = "Touchdown",
            )
        }

        if (flightState.dry && flightState.altitudeMeters > 0.0) {
            return previous.copy(
                phase = MissionPhase.FAILURE,
                missionResult = LandingOutcome.FUEL_EXHAUSTED,
                lastEvent = "Descent propellant depleted",
            )
        }

        return when {
            previous.phase == MissionPhase.BRAKING && flightState.altitudeMeters <= 850.0 -> previous.copy(
                phase = MissionPhase.APPROACH,
                lastEvent = if (alarmsOutstanding) "P64 entered under overload pressure" else "P64 approach entered",
            )
            previous.phase == MissionPhase.APPROACH && flightState.altitudeMeters <= 120.0 -> previous.copy(
                phase = MissionPhase.LANDING,
                lastEvent = "P66 landing phase entered",
            )
            else -> previous
        }
    }
}
