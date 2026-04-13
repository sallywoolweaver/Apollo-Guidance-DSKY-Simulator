package com.example.myapplication.dsky.input

import com.example.myapplication.dsky.model.Annunciator
import com.example.myapplication.dsky.model.DisplayMode
import com.example.myapplication.dsky.model.DskyState
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class DskyInputProcessorTest {
    private val processor = DskyInputProcessor()

    @Test
    fun `loads verb noun and enters landing monitor`() {
        var state = DskyState()
        state = processor.process(state, DskyKey.Verb).state
        state = processor.process(state, DskyKey.Digit(1)).state
        state = processor.process(state, DskyKey.Digit(6)).state
        state = processor.process(state, DskyKey.Enter).state
        state = processor.process(state, DskyKey.Noun).state
        state = processor.process(state, DskyKey.Digit(6)).state
        state = processor.process(state, DskyKey.Digit(8)).state
        state = processor.process(state, DskyKey.Enter).state
        state = processor.process(state, DskyKey.Enter).state

        assertEquals("16", state.verb)
        assertEquals("68", state.noun)
        assertEquals(DisplayMode.LANDING_MONITOR, state.displayMode)
    }

    @Test
    fun `invalid digit entry raises operator error`() {
        val transition = processor.process(DskyState(), DskyKey.Digit(7))
        assertTrue(transition.state.annunciators.contains(Annunciator.OPR_ERR))
        assertTrue(transition.state.annunciators.contains(Annunciator.KEY_REL))
    }
}
