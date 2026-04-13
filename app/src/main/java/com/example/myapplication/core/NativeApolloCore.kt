package com.example.myapplication.core

class NativeApolloCore {
    companion object {
        init {
            System.loadLibrary("myapplication")
        }
    }

    external fun initCore()
    external fun loadProgramImage(programImage: ByteArray): Boolean
    external fun resetScenario()
    external fun pressKey(key: String)
    external fun stepSimulation(deltaSeconds: Double)
    external fun getSnapshot(): CoreSnapshot
}
