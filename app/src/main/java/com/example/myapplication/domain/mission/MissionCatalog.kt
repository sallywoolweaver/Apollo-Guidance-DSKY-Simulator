package com.example.myapplication.domain.mission

enum class VehicleType {
    LM,
    CM,
}

enum class SourceStatus(val displayName: String) {
    FULL_SOURCE("Full source available"),
    PARTIAL_OR_RECONSTRUCTED("Partial or reconstructed"),
    COMPANION_LAYER_ONLY("Companion-layer only"),
}

enum class ScenarioSupportLevel(val displayName: String) {
    PLAYABLE("Playable"),
    SCAFFOLDED("Scaffolded"),
    PLANNED("Planned"),
}

enum class AuthenticityClassification(val displayName: String) {
    EXACT_SOURCE_BACKED_EXECUTION("Exact source-backed execution"),
    SOURCE_INFORMED_EXECUTION_WITH_COMPANION_TRANSLATION("Source-informed execution with companion translation"),
    HISTORICAL_APPROXIMATION("Historical approximation"),
    PLANNED_PLACEHOLDER("Planned / placeholder"),
}

data class MissionFamily(
    val missionId: String,
    val missionDisplayName: String,
)

data class SoftwareVariant(
    val id: String,
    val missionFamily: MissionFamily,
    val vehicleType: VehicleType,
    val softwareName: String,
    val buildNumber: String,
    val sourceStatus: SourceStatus,
    val scenarioSupportLevel: ScenarioSupportLevel,
    val authenticityClassification: AuthenticityClassification,
    val statusNote: String,
    val ropeAsset: String? = null,
    val erasableAsset: String? = null,
    val bootstrapAsset: String? = null,
)

object MissionCatalog {
    private val apollo11 = MissionFamily("apollo11", "Apollo 11")
    private val apollo13 = MissionFamily("apollo13", "Apollo 13")
    private val rendezvous = MissionFamily("rendezvous", "Rendezvous Focus")

    val variants: List<SoftwareVariant> = listOf(
        SoftwareVariant(
            id = "apollo11_lm_luminary099",
            missionFamily = apollo11,
            vehicleType = VehicleType.LM,
            softwareName = "Luminary",
            buildNumber = "099",
            sourceStatus = SourceStatus.FULL_SOURCE,
            scenarioSupportLevel = ScenarioSupportLevel.PLAYABLE,
            authenticityClassification = AuthenticityClassification.SOURCE_INFORMED_EXECUTION_WITH_COMPANION_TRANSLATION,
            statusNote = "Current vertical slice loads separated Luminary099 rope-word excerpt and bootstrap assets. Runtime behavior is still driven by a compatibility layer rather than real AGC instruction execution.",
            ropeAsset = "programs/apollo11/lm/luminary099/rope_excerpt.cfg",
            erasableAsset = "programs/apollo11/lm/luminary099/erasable_init.cfg",
            bootstrapAsset = "bootstrap/apollo11/lm/luminary099/bootstrap.cfg",
        ),
        SoftwareVariant(
            id = "apollo11_cm_comanche055",
            missionFamily = apollo11,
            vehicleType = VehicleType.CM,
            softwareName = "Comanche",
            buildNumber = "055",
            sourceStatus = SourceStatus.FULL_SOURCE,
            scenarioSupportLevel = ScenarioSupportLevel.SCAFFOLDED,
            authenticityClassification = AuthenticityClassification.PLANNED_PLACEHOLDER,
            statusNote = "Strong next candidate: source listings and binaries are available through Virtual AGC, but no CM execution path exists in this app yet.",
            ropeAsset = "programs/apollo11/cm/comanche055/rope_excerpt.cfg",
            erasableAsset = "programs/apollo11/cm/comanche055/erasable_init.cfg",
            bootstrapAsset = "bootstrap/apollo11/cm/comanche055/bootstrap.cfg",
        ),
        SoftwareVariant(
            id = "apollo13_lm_lm131r1",
            missionFamily = apollo13,
            vehicleType = VehicleType.LM,
            softwareName = "LM131 rev 1 / Luminary 131/1",
            buildNumber = "131/1",
            sourceStatus = SourceStatus.FULL_SOURCE,
            scenarioSupportLevel = ScenarioSupportLevel.SCAFFOLDED,
            authenticityClassification = AuthenticityClassification.PLANNED_PLACEHOLDER,
            statusNote = "Virtual AGC documents this as the final Apollo 13 LM flight software family, but this app does not yet load or execute it.",
        ),
        SoftwareVariant(
            id = "rendezvous_cm_family",
            missionFamily = rendezvous,
            vehicleType = VehicleType.CM,
            softwareName = "Comanche / Colossus rendezvous family",
            buildNumber = "TBD",
            sourceStatus = SourceStatus.FULL_SOURCE,
            scenarioSupportLevel = ScenarioSupportLevel.PLANNED,
            authenticityClassification = AuthenticityClassification.PLANNED_PLACEHOLDER,
            statusNote = "Rendezvous scenario support is planned as a software-family scaffold only. Exact mission/software pairing still needs selection and verification.",
        ),
    )

    fun defaultVariant(): SoftwareVariant = variants.first()
}
