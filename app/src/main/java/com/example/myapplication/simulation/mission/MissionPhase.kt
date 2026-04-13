package com.example.myapplication.simulation.mission

import com.example.myapplication.simulation.guidance.GuidanceProgram

enum class MissionPhase(
    val label: String,
    val program: GuidanceProgram,
) {
    BRAKING("Braking", GuidanceProgram.P63),
    APPROACH("Approach", GuidanceProgram.P64),
    LANDING("Landing", GuidanceProgram.P66),
    LANDED("Landed", GuidanceProgram.P68),
    FAILURE("Failure", GuidanceProgram.P68),
}
