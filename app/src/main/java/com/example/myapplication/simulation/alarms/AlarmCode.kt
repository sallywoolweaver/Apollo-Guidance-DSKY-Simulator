package com.example.myapplication.simulation.alarms

enum class AlarmCode(
    val code: String,
    val title: String,
    val authenticityNote: String,
) {
    EXECUTIVE_OVERFLOW_CORESET(
        code = "1202",
        title = "Executive Overflow (Coreset Pressure)",
        authenticityNote = "Historically grounded alarm family; exact trigger logic is simplified in this app.",
    ),
    EXECUTIVE_OVERFLOW_VAC(
        code = "1201",
        title = "Executive Overflow (VAC Pressure)",
        authenticityNote = "Historically grounded alarm family; exact trigger logic is simplified in this app.",
    ),
}
