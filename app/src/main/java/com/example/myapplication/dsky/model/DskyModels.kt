package com.example.myapplication.dsky.model

enum class Annunciator {
    PROG,
    OPR_ERR,
    COMP_ACTY,
    KEY_REL,
}

enum class EntryField {
    VERB,
    NOUN,
}

enum class DisplayMode {
    LANDING_MONITOR,
    PHASE_SUMMARY,
    LAST_ALARM,
}

data class DskyState(
    val program: String = "63",
    val verb: String? = null,
    val noun: String? = null,
    val register1: String = "+00000",
    val register2: String = "+00000",
    val register3: String = "+00000",
    val entryField: EntryField? = null,
    val entryBuffer: String = "",
    val displayMode: DisplayMode = DisplayMode.LANDING_MONITOR,
    val annunciators: Set<Annunciator> = setOf(Annunciator.COMP_ACTY),
    val statusLine: String = "Verb 16 Noun 68 monitor active",
)
