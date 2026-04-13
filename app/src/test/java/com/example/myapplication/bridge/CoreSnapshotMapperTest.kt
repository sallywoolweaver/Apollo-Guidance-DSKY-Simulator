package com.example.myapplication.bridge

import com.example.myapplication.bridge.CoreSnapshotMapper.toDskyState
import com.example.myapplication.core.CoreSnapshot
import com.example.myapplication.dsky.model.Annunciator
import com.example.myapplication.dsky.model.DisplayMode
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class CoreSnapshotMapperTest {
    @Test
    fun `maps alarm and phase summary display`() {
        val snapshot = CoreSnapshot().apply {
            verb = "06"
            noun = "64"
            program = "64"
            register1 = "+00064"
            progLight = true
            oprErrLight = true
            compActyLight = false
            keyRelLight = true
            statusLine = "Phase summary"
        }

        val dsky = snapshot.toDskyState()

        assertEquals(DisplayMode.PHASE_SUMMARY, dsky.displayMode)
        assertTrue(dsky.annunciators.contains(Annunciator.PROG))
        assertTrue(dsky.annunciators.contains(Annunciator.OPR_ERR))
        assertTrue(dsky.annunciators.contains(Annunciator.KEY_REL))
    }
}
