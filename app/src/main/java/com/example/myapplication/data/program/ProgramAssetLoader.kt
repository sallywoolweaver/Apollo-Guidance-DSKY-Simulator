package com.example.myapplication.data.program

import android.content.res.AssetManager
import com.example.myapplication.domain.mission.SoftwareVariant

class ProgramAssetLoader(
    private val assetManager: AssetManager,
) {
    fun loadPackage(variant: SoftwareVariant): ByteArray {
        val ropePath = variant.ropeAsset ?: error("No rope asset configured for ${variant.id}")
        val erasablePath = variant.erasableAsset ?: error("No erasable asset configured for ${variant.id}")
        val bootstrapPath = variant.bootstrapAsset ?: error("No bootstrap asset configured for ${variant.id}")
        val rope = assetManager.open(ropePath).bufferedReader().use { it.readText() }
        val erasable = assetManager.open(erasablePath).bufferedReader().use { it.readText() }
        val bootstrap = assetManager.open(bootstrapPath).bufferedReader().use { it.readText() }
        val packaged = buildString {
            appendLine("[[ROPE]]")
            appendLine(rope.trimEnd())
            appendLine("[[ERASABLE]]")
            appendLine(erasable.trimEnd())
            appendLine("[[BOOTSTRAP]]")
            appendLine(bootstrap.trimEnd())
        }
        return packaged.toByteArray()
    }
}
