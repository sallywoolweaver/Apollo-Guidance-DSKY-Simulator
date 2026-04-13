package com.example.myapplication.simulation.flight

enum class LandingOutcome(val title: String, val summary: String) {
    SAFE(
        title = "Safe Landing",
        summary = "Touchdown rates stayed within the slice's conservative landing envelope.",
    ),
    HARD(
        title = "Hard Landing",
        summary = "Touchdown was survivable in this simulator, but outside nominal comfort.",
    ),
    CRASH(
        title = "Crash",
        summary = "Impact energy exceeded the simulator's safe touchdown thresholds.",
    ),
    FUEL_EXHAUSTED(
        title = "Fuel Exhausted",
        summary = "Usable descent propellant was exhausted before a controlled landing.",
    ),
}
