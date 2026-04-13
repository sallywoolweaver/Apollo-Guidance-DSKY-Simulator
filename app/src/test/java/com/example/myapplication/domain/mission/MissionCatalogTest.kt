package com.example.myapplication.domain.mission

import org.junit.Assert.assertEquals
import org.junit.Assert.assertNotNull
import org.junit.Test

class MissionCatalogTest {
    @Test
    fun `default variant is current playable lm slice`() {
        val variant = MissionCatalog.defaultVariant()

        assertEquals("apollo11_lm_luminary099", variant.id)
        assertEquals(ScenarioSupportLevel.PLAYABLE, variant.scenarioSupportLevel)
        assertNotNull(variant.ropeAsset)
        assertNotNull(variant.erasableAsset)
        assertNotNull(variant.bootstrapAsset)
    }
}
