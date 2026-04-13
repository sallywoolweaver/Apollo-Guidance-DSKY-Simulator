package com.example.myapplication.simulation.alarms

import com.example.myapplication.domain.RealismProfile
import com.example.myapplication.simulation.mission.MissionPhase

data class OverloadSnapshot(
    val loadRatio: Double,
    val activeAlarm: AlarmCode?,
)

class OverloadMonitor(
    private val realismProfile: RealismProfile,
) {
    fun evaluate(
        elapsedSeconds: Double,
        phase: MissionPhase,
        altitudeMeters: Double,
        monitorModeActive: Boolean,
        recentInputEvents: Int,
        alarmState: AlarmState,
    ): OverloadSnapshot {
        val phaseLoad = when (phase) {
            MissionPhase.BRAKING -> 0.72
            MissionPhase.APPROACH -> 0.81
            MissionPhase.LANDING -> 0.56
            MissionPhase.LANDED, MissionPhase.FAILURE -> 0.18
        }
        val displayLoad = if (monitorModeActive) 0.16 else 0.05
        val inputLoad = (recentInputEvents * 0.025).coerceAtMost(0.16)
        val radarLoad = if (altitudeMeters in 450.0..1800.0 && phase != MissionPhase.LANDING) 0.17 else 0.0
        val profileAdjustment = -realismProfile.loadMargin
        val scriptedPressure = when {
            elapsedSeconds in 32.0..37.0 -> 0.14
            elapsedSeconds in 52.0..57.0 -> 0.15
            elapsedSeconds in 88.0..93.0 -> 0.22
            elapsedSeconds in 102.0..108.0 -> 0.16
            elapsedSeconds in 114.0..120.0 -> 0.17
            else -> 0.0
        }

        val totalLoad = phaseLoad + displayLoad + inputLoad + radarLoad + scriptedPressure + profileAdjustment
        val activeAlarm = when {
            alarmState.needsAcknowledgement -> alarmState.active
            totalLoad > 1.22 && phase == MissionPhase.APPROACH -> AlarmCode.EXECUTIVE_OVERFLOW_VAC
            totalLoad > 1.08 -> AlarmCode.EXECUTIVE_OVERFLOW_CORESET
            else -> null
        }
        return OverloadSnapshot(loadRatio = totalLoad, activeAlarm = activeAlarm)
    }
}
