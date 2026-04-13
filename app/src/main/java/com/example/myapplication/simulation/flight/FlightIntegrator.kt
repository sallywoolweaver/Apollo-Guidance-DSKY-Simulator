package com.example.myapplication.simulation.flight

import com.example.myapplication.domain.RealismProfile
import com.example.myapplication.simulation.mission.MissionPhase
import kotlin.math.abs
import kotlin.math.max

data class GuidanceSolution(
    val commandedThrottle: Double,
    val targetVerticalVelocity: Double,
)

class FlightIntegrator(
    private val realismProfile: RealismProfile,
) {
    private val lunarGravity = 1.62
    private val maxUpwardAcceleration = 5.35
    private val fullThrottleFuelFlowKgPerSecond = 5.4

    fun guidanceFor(
        phase: MissionPhase,
        altitudeMeters: Double,
        verticalVelocityMps: Double,
        throttleBias: Double,
    ): GuidanceSolution {
        val targetVerticalVelocity = when (phase) {
            MissionPhase.BRAKING -> -(12.0 + altitudeMeters / 95.0).coerceIn(16.0, 42.0)
            MissionPhase.APPROACH -> -(3.0 + altitudeMeters / 110.0).coerceIn(4.0, 14.0)
            MissionPhase.LANDING -> -(0.7 + altitudeMeters / 180.0).coerceIn(1.0, 4.2)
            MissionPhase.LANDED, MissionPhase.FAILURE -> 0.0
        }

        val error = targetVerticalVelocity - verticalVelocityMps
        val baseThrottle = ((lunarGravity + error * 0.12) / maxUpwardAcceleration).coerceIn(0.0, 1.0)
        val adjustedThrottle = (baseThrottle + throttleBias * realismProfile.throttleAuthority).coerceIn(0.0, 1.0)

        return GuidanceSolution(adjustedThrottle, targetVerticalVelocity)
    }

    fun step(
        previous: FlightState,
        phase: MissionPhase,
        throttleBias: Double,
        deltaSeconds: Double,
    ): Pair<FlightState, GuidanceSolution> {
        val solution = guidanceFor(
            phase = phase,
            altitudeMeters = previous.altitudeMeters,
            verticalVelocityMps = previous.verticalVelocityMps,
            throttleBias = throttleBias,
        )
        val availableThrottle = if (previous.fuelKg <= 0.0) 0.0 else solution.commandedThrottle
        val netAcceleration = availableThrottle * maxUpwardAcceleration - lunarGravity
        val nextVelocity = previous.verticalVelocityMps + netAcceleration * deltaSeconds
        val nextAltitude = (previous.altitudeMeters + nextVelocity * deltaSeconds).coerceAtLeast(0.0)
        val fuelBurn = fullThrottleFuelFlowKgPerSecond * availableThrottle * deltaSeconds
        val nextFuel = (previous.fuelKg - fuelBurn).coerceAtLeast(0.0)
        val horizontalDamping = when (phase) {
            MissionPhase.BRAKING -> 0.010
            MissionPhase.APPROACH -> 0.018
            MissionPhase.LANDING -> 0.028
            MissionPhase.LANDED, MissionPhase.FAILURE -> 0.0
        }
        val nextHorizontal = if (nextAltitude <= 0.0) {
            previous.horizontalVelocityMps
        } else {
            max(0.0, previous.horizontalVelocityMps - horizontalDamping * deltaSeconds * (1.0 + availableThrottle))
        }
        val contact = nextAltitude <= 0.0 && abs(nextVelocity) > 0.0

        return FlightState(
            altitudeMeters = nextAltitude,
            verticalVelocityMps = nextVelocity,
            horizontalVelocityMps = nextHorizontal,
            fuelKg = nextFuel,
            throttle = availableThrottle,
            missionTimeSeconds = previous.missionTimeSeconds + deltaSeconds,
            contact = contact,
        ) to solution
    }
}
