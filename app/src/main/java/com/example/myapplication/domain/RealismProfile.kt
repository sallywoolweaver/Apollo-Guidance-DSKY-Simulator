package com.example.myapplication.domain

enum class RealismProfile(
    val displayName: String,
    val description: String,
    val loadMargin: Double,
    val throttleAuthority: Double,
) {
    ASSISTED(
        displayName = "Assisted",
        description = "Forgiving throttle authority and softer overload thresholds.",
        loadMargin = 0.12,
        throttleAuthority = 0.22,
    ),
    HISTORICAL_INSPIRED(
        displayName = "Historical-Inspired",
        description = "Best balance between tension, clarity, and historical framing.",
        loadMargin = 0.0,
        throttleAuthority = 0.16,
    ),
    STRICT_SIMULATION(
        displayName = "Strict Simulation",
        description = "Minimal affordances and tighter overload tolerance.",
        loadMargin = -0.08,
        throttleAuthority = 0.12,
    ),
}
