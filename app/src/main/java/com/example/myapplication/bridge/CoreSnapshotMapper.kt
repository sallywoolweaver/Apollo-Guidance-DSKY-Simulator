package com.example.myapplication.bridge

import com.example.myapplication.core.CoreSnapshot
import com.example.myapplication.dsky.model.Annunciator
import com.example.myapplication.dsky.model.DisplayMode
import com.example.myapplication.dsky.model.DskyState

object CoreSnapshotMapper {
    fun CoreSnapshot.toDskyState(): DskyState {
        val annunciators = buildSet {
            if (progLight) add(Annunciator.PROG)
            if (oprErrLight) add(Annunciator.OPR_ERR)
            if (compActyLight) add(Annunciator.COMP_ACTY)
            if (keyRelLight) add(Annunciator.KEY_REL)
        }
        val displayMode = when {
            verb == "05" && noun == "09" -> DisplayMode.LAST_ALARM
            verb == "06" -> DisplayMode.PHASE_SUMMARY
            else -> DisplayMode.LANDING_MONITOR
        }
        return DskyState(
            program = program,
            verb = verb.ifEmpty { null },
            noun = noun.ifEmpty { null },
            register1 = register1,
            register2 = register2,
            register3 = register3,
            displayMode = displayMode,
            annunciators = annunciators,
            statusLine = statusLine,
        )
    }
}
