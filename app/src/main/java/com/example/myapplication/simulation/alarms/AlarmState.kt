package com.example.myapplication.simulation.alarms

data class AlarmState(
    val active: AlarmCode? = null,
    val lastTriggered: AlarmCode? = null,
    val needsAcknowledgement: Boolean = false,
    val totalTriggered: Int = 0,
)
