package com.example.myapplication.data.program

import android.content.res.AssetManager
import com.example.myapplication.domain.mission.SoftwareVariant

class ProgramAssetLoader(
    private val assetManager: AssetManager,
) {
    private val octalWordRegex = Regex("^[0-7]+$")

    fun loadPackage(variant: SoftwareVariant): ByteArray {
        val ropePath = variant.ropeAsset ?: error("No rope asset configured for ${variant.id}")
        val ropeLabelPath = variant.ropeLabelAsset
        val erasablePath = variant.erasableAsset ?: error("No erasable asset configured for ${variant.id}")
        val bootstrapPath = variant.bootstrapAsset ?: error("No bootstrap asset configured for ${variant.id}")

        val ropeLabelsText = ropeLabelPath?.let { path ->
            assetManager.open(path).bufferedReader().use { it.readText().trimEnd() }
        }
        val normalizedRopeLabelsText = ropeLabelsText?.let(::normalizeRopeLabelOverlayText)
        val ropeSection = loadRopeSection(ropePath, entryPoint = normalizedRopeLabelsText?.let(::findFirstRopeWordAddress))
        val ropeLabels = normalizedRopeLabelsText?.toByteArray()
        val erasable = assetManager.open(erasablePath).bufferedReader().use { it.readText().trimEnd().toByteArray() }
        val bootstrap = assetManager.open(bootstrapPath).bufferedReader().use { it.readText().trimEnd().toByteArray() }

        return buildLengthDelimitedPackage(
            listOfNotNull(
                ropeSection.sectionName to ropeSection.payload,
                "ROPE_METADATA_TEXT" to ropeSection.metadata.toByteArray(),
                ropeLabels?.let { "ROPE_LABELS_TEXT" to it },
                "ERASABLE_TEXT" to erasable,
                "BOOTSTRAP_TEXT" to bootstrap,
            ),
        )
    }

    private fun loadRopeSection(ropePath: String, entryPoint: RopeWordAddress?): RopeSection {
        if (ropePath.endsWith(".binsource", ignoreCase = true)) {
            val binsource = assetManager.open(ropePath).bufferedReader().use { it.readText() }
            val parsed = parseBinsourceToRopeBinary(binsource)
            val effectiveEntryPoint = entryPoint ?: RopeWordAddress(parsed.firstBank, parsed.firstOffset)
            val metadata = buildString {
                appendLine("image_type=apollo_program_rope_binary")
                appendLine("source_name=${ropePath.substringAfterLast('/').substringBeforeLast('.')}")
                appendLine("source_kind=apollo-binsource-derived-rope")
                appendLine("first_rope_bank=${effectiveEntryPoint.bank.toString(8).padStart(2, '0')}")
                appendLine("first_rope_offset=${effectiveEntryPoint.offset.toString(8).padStart(4, '0')}")
            }.trimEnd()
            return RopeSection("ROPE_BIN", parsed.bytes, metadata)
        }

        val ropeText = assetManager.open(ropePath).bufferedReader().use { it.readText().trimEnd() }
        return RopeSection(
            sectionName = "ROPE_TEXT",
            payload = ropeText.toByteArray(),
            metadata = "",
        )
    }

    private fun buildLengthDelimitedPackage(sections: List<Pair<String, ByteArray>>): ByteArray {
        val output = ArrayList<Byte>()
        appendAscii(output, "APOLLOPKG\n")
        sections.forEach { (name, payload) ->
            appendAscii(output, "$name ${payload.size}\n")
            payload.forEach(output::add)
        }
        return output.toByteArray()
    }

    private fun parseBinsourceToRopeBinary(text: String): ParsedRopeBinary {
        val words = IntArray(36 * 1024)
        var currentBank = -1
        var useParity = false
        var firstBank = -1
        var firstOffset = -1

        text.lineSequence().forEach { rawLine ->
            val line = rawLine.trim()
            if (line.isEmpty() || line.startsWith(";")) {
                return@forEach
            }
            when {
                line.startsWith("PARITY=") -> {
                    useParity = line.substringAfter('=').trim() != "0"
                }
                line.startsWith("BANK=") -> {
                    currentBank = line.substringAfter('=').trim().toInt(radix = 8)
                }
                line.startsWith("NUMBANKS=") || line.startsWith("CHECKWORDS=") || line.startsWith("BUGGER=") -> {
                }
                else -> {
                    require(currentBank >= 0) { "Encountered rope words before BANK= in $line" }
                    val tokens = line.split(Regex("[\\s,]+")).filter { it.isNotBlank() }
                    if (tokens.isEmpty()) {
                        return@forEach
                    }
                    val hasAddressPrefix = tokens.size >= 2 &&
                        tokens.first().length == 6 &&
                        octalWordRegex.matches(tokens.first())
                    var offset = if (hasAddressPrefix) tokens.first().toInt(radix = 8) else 0
                    val dataTokens = if (hasAddressPrefix) tokens.drop(1) else tokens
                    dataTokens.forEach { token ->
                        if (!octalWordRegex.matches(token)) {
                            return@forEach
                        }
                        val parsed = token.toInt(radix = 8)
                        val normalized = if (useParity) parsed shr 3 else parsed
                        val word = (normalized shl 1) and 0xFFFF
                        val index = currentBank * 1024 + (offset and 0x3FF)
                        require(index in words.indices) { "Rope word index $index out of range for bank $currentBank offset $offset" }
                        words[index] = word
                        if (firstBank < 0) {
                            firstBank = currentBank
                            firstOffset = offset
                        }
                        offset += 1
                    }
                }
            }
        }

        require(firstBank >= 0 && firstOffset >= 0) { "No rope words parsed from binsource asset" }

        val output = ByteArray(words.size * 2)
        words.forEachIndexed { index, word ->
            output[index * 2] = ((word shr 8) and 0xFF).toByte()
            output[index * 2 + 1] = (word and 0xFF).toByte()
        }
        return ParsedRopeBinary(output, firstBank, firstOffset)
    }

    private fun appendAscii(output: MutableList<Byte>, text: String) {
        text.toByteArray().forEach(output::add)
    }

    private fun findFirstRopeWordAddress(text: String): RopeWordAddress? {
        text.lineSequence().forEach { rawLine ->
            val line = rawLine.trim()
            if (line.isEmpty() || line.startsWith("#")) {
                return@forEach
            }
            val delimiter = line.indexOf('=')
            if (delimiter <= 0 || line.substring(0, delimiter).trim() != "rope_word") {
                return@forEach
            }
            val parts = line.substring(delimiter + 1).trim().split(':')
            if (parts.size >= 2) {
                return RopeWordAddress(
                    bank = parts[0].toInt(radix = 8),
                    offset = parts[1].toInt(radix = 8),
                )
            }
        }
        return null
    }

    private fun normalizeRopeLabelOverlayText(text: String): String {
        return text.lineSequence()
            .map { rawLine ->
                val line = rawLine.trim()
                if (line.isEmpty() || line.startsWith("#")) {
                    return@map rawLine
                }
                val delimiter = line.indexOf('=')
                if (delimiter <= 0 || line.substring(0, delimiter).trim() != "rope_word") {
                    return@map rawLine
                }
                val parts = line.substring(delimiter + 1).trim().split(':').toMutableList()
                if (parts.size < 3) {
                    return@map rawLine
                }
                val bank = parts[0].toInt(radix = 8)
                val offset = parts[1].toInt(radix = 8)
                parts[1] = normalizeRopeOffset(bank, offset).toString(8).padStart(4, '0')
                "rope_word=${parts.joinToString(":")}"
            }
            .joinToString("\n")
    }

    private fun normalizeRopeOffset(bank: Int, offset: Int): Int {
        return when {
            bank == 2 && offset >= 2048 -> offset - 2048
            bank == 3 && offset >= 3072 -> offset - 3072
            bank >= 4 && offset >= 1024 -> offset - 1024
            else -> offset
        }
    }
}

private data class RopeSection(
    val sectionName: String,
    val payload: ByteArray,
    val metadata: String,
)

private data class ParsedRopeBinary(
    val bytes: ByteArray,
    val firstBank: Int,
    val firstOffset: Int,
)

private data class RopeWordAddress(
    val bank: Int,
    val offset: Int,
)
