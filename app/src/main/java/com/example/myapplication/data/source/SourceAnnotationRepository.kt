package com.example.myapplication.data.source

import android.content.res.AssetManager
import org.json.JSONArray
import org.json.JSONObject

class SourceAnnotationRepository(
    private val assetManager: AssetManager,
) {
    fun load(): SourceAnnotationCatalog {
        val json = assetManager.open("source_annotations/source_annotations.json").bufferedReader().use { it.readText() }
        val root = JSONObject(json)
        val labelsJson = root.getJSONObject("labels")
        val filesJson = root.getJSONObject("files")
        val tagsJson = root.getJSONObject("tags")

        val labels = buildMap {
            labelsJson.keys().forEach { id ->
                val value = labelsJson.getJSONObject(id)
                put(
                    id,
                    SourceLabelInfo(
                        id = id,
                        file = value.getString("file"),
                        title = value.getString("title"),
                        summary = value.getString("summary"),
                        commentExcerpt = value.getString("commentExcerpt"),
                        commentBlock = value.getString("commentBlock"),
                        tags = value.getJSONArray("tags").toStringList(),
                        sourceSection = value.optString("sourceSection", ""),
                        bank = value.optNullableInt("bank"),
                        offset = value.optNullableInt("offset"),
                        mappingNotes = value.optString("mappingNotes", ""),
                    ),
                )
            }
        }

        val files = buildMap {
            filesJson.keys().forEach { path ->
                val value = filesJson.getJSONObject(path)
                put(
                    path,
                    SourceFileInfo(
                        path = path,
                        title = value.getString("title"),
                        headerComment = value.getString("headerComment"),
                        labels = value.getJSONArray("labels").toStringList(),
                    ),
                )
            }
        }

        val tags = buildMap {
            tagsJson.keys().forEach { tag ->
                put(tag, tagsJson.getJSONArray(tag).toStringList())
            }
        }

        return SourceAnnotationCatalog(
            labels = labels,
            files = files,
            tags = tags,
            labelsByAddress = labels.values.mapNotNull { label ->
                val bank = label.bank ?: return@mapNotNull null
                val offset = label.offset ?: return@mapNotNull null
                "${bank.toString(8).padStart(2, '0')}:${offset.toString(8).padStart(4, '0')}" to label
            }.toMap(),
        )
    }
}

private fun JSONArray.toStringList(): List<String> = buildList {
    for (index in 0 until length()) {
        add(getString(index))
    }
}

private fun JSONObject.optNullableInt(name: String): Int? {
    if (!has(name) || isNull(name)) {
        return null
    }
    return getInt(name)
}
