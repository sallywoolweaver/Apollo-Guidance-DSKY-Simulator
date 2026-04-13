package com.example.myapplication.simulation.alarms

import com.example.myapplication.domain.RealismProfile
import com.example.myapplication.simulation.mission.MissionPhase
import org.junit.Assert.assertEquals
import org.junit.Test

class OverloadMonitorTest {
    @Test
    fun `p63 scripted pressure can raise 1202`() {
        val monitor = OverloadMonitor(RealismProfile.HISTORICAL_INSPIRED)

        val snapshot = monitor.evaluate(
            elapsedSeconds = 34.0,
            phase = MissionPhase.BRAKING,
            altitudeMeters = 1500.0,
            monitorModeActive = true,
            recentInputEvents = 2,
            alarmState = AlarmState(),
        )

        assertEquals(AlarmCode.EXECUTIVE_OVERFLOW_CORESET, snapshot.activeAlarm)
    }

    @Test
    fun `p64 high pressure can raise 1201`() {
        val monitor = OverloadMonitor(RealismProfile.HISTORICAL_INSPIRED)

        val snapshot = monitor.evaluate(
            elapsedSeconds = 90.0,
            phase = MissionPhase.APPROACH,
            altitudeMeters = 600.0,
            monitorModeActive = true,
            recentInputEvents = 4,
            alarmState = AlarmState(),
        )

        assertEquals(AlarmCode.EXECUTIVE_OVERFLOW_VAC, snapshot.activeAlarm)
    }
}
