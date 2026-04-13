package com.example.myapplication.core

class CoreSnapshot {
    @JvmField var program: String = "63"
    @JvmField var verb: String = "16"
    @JvmField var noun: String = "68"
    @JvmField var register1: String = "+02400"
    @JvmField var register2: String = "-00340"
    @JvmField var register3: String = "+00835"
    @JvmField var phaseLabel: String = "Braking"
    @JvmField var phaseProgram: String = "63"
    @JvmField var statusLine: String = "Native core idle"
    @JvmField var activeAlarmCode: String = ""
    @JvmField var activeAlarmTitle: String = ""
    @JvmField var alarmNeedsAcknowledgement: Boolean = false
    @JvmField var totalAlarms: Int = 0
    @JvmField var altitudeMeters: Double = 2400.0
    @JvmField var verticalVelocityMps: Double = -34.0
    @JvmField var horizontalVelocityMps: Double = 24.0
    @JvmField var fuelKg: Double = 835.0
    @JvmField var throttle: Double = 0.62
    @JvmField var targetVerticalVelocityMps: Double = -30.0
    @JvmField var missionTimeSeconds: Double = 0.0
    @JvmField var loadRatio: Double = 0.0
    @JvmField var throttleTrim: Double = 0.0
    @JvmField var missionResult: String = ""
    @JvmField var missionResultSummary: String = ""
    @JvmField var lastEvent: String = "Awaiting program image"
    @JvmField var progLight: Boolean = false
    @JvmField var oprErrLight: Boolean = false
    @JvmField var compActyLight: Boolean = true
    @JvmField var keyRelLight: Boolean = false

    companion object {
        fun initial(): CoreSnapshot = CoreSnapshot()
    }
}
