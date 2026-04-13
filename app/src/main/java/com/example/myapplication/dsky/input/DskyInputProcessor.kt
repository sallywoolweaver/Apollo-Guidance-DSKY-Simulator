package com.example.myapplication.dsky.input

import com.example.myapplication.dsky.model.Annunciator
import com.example.myapplication.dsky.model.DisplayMode
import com.example.myapplication.dsky.model.DskyState
import com.example.myapplication.dsky.model.EntryField

sealed class DskyKey(val label: String) {
    data object Verb : DskyKey("VERB")
    data object Noun : DskyKey("NOUN")
    data object Enter : DskyKey("ENTR")
    data object Clear : DskyKey("CLR")
    data object Proceed : DskyKey("PRO")
    data object KeyRelease : DskyKey("KEY REL")
    data object Reset : DskyKey("RSET")
    data class Digit(val value: Int) : DskyKey(value.toString())
}

sealed class DskyAction {
    data object None : DskyAction()
    data object AcknowledgeAlarm : DskyAction()
    data object ResetAnnunciators : DskyAction()
    data object StartProgram63 : DskyAction()
}

data class DskyTransition(
    val state: DskyState,
    val action: DskyAction,
)

class DskyInputProcessor {
    fun process(
        state: DskyState,
        key: DskyKey,
    ): DskyTransition {
        return when (key) {
            is DskyKey.Digit -> appendDigit(state, key.value)
            DskyKey.Verb -> DskyTransition(
                state.copy(entryField = EntryField.VERB, entryBuffer = "", statusLine = "Verb entry"),
                DskyAction.None,
            )
            DskyKey.Noun -> DskyTransition(
                state.copy(entryField = EntryField.NOUN, entryBuffer = "", statusLine = "Noun entry"),
                DskyAction.None,
            )
            DskyKey.Clear -> clear(state)
            DskyKey.Enter -> enter(state)
            DskyKey.Proceed -> DskyTransition(
                state.copy(statusLine = "Proceed"),
                DskyAction.AcknowledgeAlarm,
            )
            DskyKey.KeyRelease -> DskyTransition(
                state.copy(
                    annunciators = state.annunciators - Annunciator.KEY_REL - Annunciator.OPR_ERR,
                    statusLine = "Key release",
                ),
                DskyAction.None,
            )
            DskyKey.Reset -> DskyTransition(
                state.copy(
                    annunciators = state.annunciators - Annunciator.PROG - Annunciator.OPR_ERR,
                    statusLine = "Reset",
                ),
                DskyAction.ResetAnnunciators,
            )
        }
    }

    private fun appendDigit(state: DskyState, digit: Int): DskyTransition {
        val field = state.entryField ?: return invalid(state, "Select VERB or NOUN first")
        if (state.entryBuffer.length >= 2) {
            return invalid(state, "${field.name} complete")
        }
        return DskyTransition(
            state.copy(
                entryBuffer = state.entryBuffer + digit,
                annunciators = state.annunciators + Annunciator.COMP_ACTY,
            ),
            DskyAction.None,
        )
    }

    private fun clear(state: DskyState): DskyTransition {
        if (state.entryField == null) {
            return DskyTransition(state.copy(statusLine = "Idle"), DskyAction.None)
        }
        return DskyTransition(state.copy(entryBuffer = state.entryBuffer.dropLast(1), statusLine = "Clear"), DskyAction.None)
    }

    private fun enter(state: DskyState): DskyTransition {
        return when (state.entryField) {
            EntryField.VERB -> commitVerb(state)
            EntryField.NOUN -> commitNoun(state)
            null -> executeCommand(state)
        }
    }

    private fun commitVerb(state: DskyState): DskyTransition {
        if (state.entryBuffer.length != 2) {
            return invalid(state, "Verb requires 2 digits")
        }
        return DskyTransition(
            state.copy(
                verb = state.entryBuffer,
                entryField = null,
                entryBuffer = "",
                statusLine = "Verb ${state.entryBuffer} loaded",
            ),
            DskyAction.None,
        )
    }

    private fun commitNoun(state: DskyState): DskyTransition {
        if (state.entryBuffer.length != 2) {
            return invalid(state, "Noun requires 2 digits")
        }
        return DskyTransition(
            state.copy(
                noun = state.entryBuffer,
                entryField = null,
                entryBuffer = "",
                statusLine = "Noun ${state.entryBuffer} loaded",
            ),
            DskyAction.None,
        )
    }

    private fun executeCommand(state: DskyState): DskyTransition {
        return when {
            state.verb == "16" && state.noun == "68" -> DskyTransition(
                state.copy(displayMode = DisplayMode.LANDING_MONITOR, statusLine = "V16 N68 monitor"),
                DskyAction.None,
            )
            state.verb == "06" && state.noun in setOf("63", "64", "60") -> DskyTransition(
                state.copy(displayMode = DisplayMode.PHASE_SUMMARY, statusLine = "Phase summary"),
                DskyAction.None,
            )
            state.verb == "05" && state.noun == "09" -> DskyTransition(
                state.copy(displayMode = DisplayMode.LAST_ALARM, statusLine = "Last alarm display"),
                DskyAction.None,
            )
            state.verb == "37" && state.noun == "63" -> DskyTransition(
                state.copy(program = "63", statusLine = "P63 requested"),
                DskyAction.StartProgram63,
            )
            else -> invalid(state, "Unsupported command")
        }
    }

    private fun invalid(state: DskyState, reason: String): DskyTransition {
        return DskyTransition(
            state.copy(
                annunciators = state.annunciators + Annunciator.OPR_ERR + Annunciator.KEY_REL,
                statusLine = reason,
            ),
            DskyAction.None,
        )
    }
}
