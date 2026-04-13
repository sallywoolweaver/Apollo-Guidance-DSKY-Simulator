package com.example.myapplication.data.source

import com.example.myapplication.core.CoreSnapshot

data class SourceAnnotationCatalog(
    val labels: Map<String, SourceLabelInfo> = emptyMap(),
    val files: Map<String, SourceFileInfo> = emptyMap(),
    val tags: Map<String, List<String>> = emptyMap(),
)

data class SourceFileInfo(
    val path: String,
    val title: String,
    val headerComment: String,
    val labels: List<String>,
)

data class SourceLabelInfo(
    val id: String,
    val file: String,
    val title: String,
    val summary: String,
    val commentExcerpt: String,
    val commentBlock: String,
    val tags: List<String>,
)

enum class SourceRelationTier(val displayName: String) {
    CURRENTLY_EXECUTING("Currently executing"),
    RELATED_TO_CURRENT_PHASE("Related to current phase"),
    HISTORICAL_BACKGROUND_ONLY("Historical background only"),
}

data class RuntimeSourceNote(
    val relationTier: SourceRelationTier,
    val labelInfo: SourceLabelInfo,
    val modernContext: String,
)

data class SourceAnnotationState(
    val catalog: SourceAnnotationCatalog = SourceAnnotationCatalog(),
    val engineerModeEnabled: Boolean = false,
    val sourcePanelExpanded: Boolean = false,
    val currentNote: RuntimeSourceNote? = null,
    val currentSnapshotProgram: String = "",
)

object SourceAnnotationMapper {
    fun map(snapshot: CoreSnapshot, catalog: SourceAnnotationCatalog): RuntimeSourceNote? {
        fun label(id: String) = catalog.labels[id]

        return when {
            snapshot.activeAlarmCode.isNotEmpty() -> label("VARALARM")?.let {
                RuntimeSourceNote(
                    relationTier = SourceRelationTier.RELATED_TO_CURRENT_PHASE,
                    labelInfo = it,
                    modernContext = "The simulator is showing a program-alarm condition. This note is related background from the LM alarm path, not a claim that VARALARM is executing exactly.",
                )
            }
            snapshot.phaseProgram == "63" -> label("P63LM")?.let {
                RuntimeSourceNote(
                    relationTier = SourceRelationTier.RELATED_TO_CURRENT_PHASE,
                    labelInfo = it,
                    modernContext = "The simulator is in the P63 braking-phase slice. This note is related to the current phase and setup behavior.",
                )
            }
            snapshot.phaseProgram == "64" || snapshot.phaseProgram == "66" -> label("NEWPHASE")?.let {
                RuntimeSourceNote(
                    relationTier = SourceRelationTier.RELATED_TO_CURRENT_PHASE,
                    labelInfo = it,
                    modernContext = "The simulator is in a later powered-descent phase. This note points to the guidance phase-dispatch structure rather than an exact live routine binding.",
                )
            }
            snapshot.verb.isNotEmpty() || snapshot.noun.isNotEmpty() -> label("CHARIN")?.let {
                RuntimeSourceNote(
                    relationTier = SourceRelationTier.HISTORICAL_BACKGROUND_ONLY,
                    labelInfo = it,
                    modernContext = "This note provides historical DSKY background for keypad and verb/noun interaction.",
                )
            }
            else -> null
        }
    }
}
