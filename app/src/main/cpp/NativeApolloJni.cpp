#include <jni.h>

#include "NativeApolloCore.h"

using apollo::CoreState;
using apollo::NativeApolloCore;

namespace {
NativeApolloCore& core() {
    static NativeApolloCore instance;
    return instance;
}

void setStringField(JNIEnv* env, jobject target, jclass clazz, const char* name, const std::string& value) {
    const jfieldID field = env->GetFieldID(clazz, name, "Ljava/lang/String;");
    env->SetObjectField(target, field, env->NewStringUTF(value.c_str()));
}

void setBooleanField(JNIEnv* env, jobject target, jclass clazz, const char* name, bool value) {
    const jfieldID field = env->GetFieldID(clazz, name, "Z");
    env->SetBooleanField(target, field, value);
}

void setIntField(JNIEnv* env, jobject target, jclass clazz, const char* name, int value) {
    const jfieldID field = env->GetFieldID(clazz, name, "I");
    env->SetIntField(target, field, value);
}

void setDoubleField(JNIEnv* env, jobject target, jclass clazz, const char* name, double value) {
    const jfieldID field = env->GetFieldID(clazz, name, "D");
    env->SetDoubleField(target, field, value);
}

jobject toSnapshot(JNIEnv* env, const CoreState& snapshot) {
    jclass clazz = env->FindClass("com/example/myapplication/core/CoreSnapshot");
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    jobject object = env->NewObject(clazz, constructor);

    setStringField(env, object, clazz, "program", snapshot.program);
    setStringField(env, object, clazz, "verb", snapshot.verb);
    setStringField(env, object, clazz, "noun", snapshot.noun);
    setStringField(env, object, clazz, "register1", snapshot.register1);
    setStringField(env, object, clazz, "register2", snapshot.register2);
    setStringField(env, object, clazz, "register3", snapshot.register3);
    setStringField(env, object, clazz, "phaseLabel", snapshot.phaseLabel);
    setStringField(env, object, clazz, "phaseProgram", snapshot.phaseProgram);
    setStringField(env, object, clazz, "statusLine", snapshot.statusLine);
    setStringField(env, object, clazz, "activeAlarmCode", snapshot.activeAlarmCode);
    setStringField(env, object, clazz, "activeAlarmTitle", snapshot.activeAlarmTitle);
    setBooleanField(env, object, clazz, "alarmNeedsAcknowledgement", snapshot.alarmNeedsAcknowledgement);
    setIntField(env, object, clazz, "totalAlarms", snapshot.totalAlarms);
    setDoubleField(env, object, clazz, "altitudeMeters", snapshot.altitudeMeters);
    setDoubleField(env, object, clazz, "verticalVelocityMps", snapshot.verticalVelocityMps);
    setDoubleField(env, object, clazz, "horizontalVelocityMps", snapshot.horizontalVelocityMps);
    setDoubleField(env, object, clazz, "fuelKg", snapshot.fuelKg);
    setDoubleField(env, object, clazz, "throttle", snapshot.throttle);
    setDoubleField(env, object, clazz, "targetVerticalVelocityMps", snapshot.targetVerticalVelocityMps);
    setDoubleField(env, object, clazz, "missionTimeSeconds", snapshot.missionTimeSeconds);
    setDoubleField(env, object, clazz, "loadRatio", snapshot.loadRatio);
    setDoubleField(env, object, clazz, "throttleTrim", snapshot.throttleTrim);
    setStringField(env, object, clazz, "missionResult", snapshot.missionResult);
    setStringField(env, object, clazz, "missionResultSummary", snapshot.missionResultSummary);
    setStringField(env, object, clazz, "lastEvent", snapshot.lastEvent);
    setBooleanField(env, object, clazz, "progLight", snapshot.progLight);
    setBooleanField(env, object, clazz, "oprErrLight", snapshot.oprErrLight);
    setBooleanField(env, object, clazz, "compActyLight", snapshot.compActyLight);
    setBooleanField(env, object, clazz, "keyRelLight", snapshot.keyRelLight);
    return object;
}
}  // namespace

extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_core_NativeApolloCore_initCore(JNIEnv*, jobject) {
    core().initCore();
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_myapplication_core_NativeApolloCore_loadProgramImage(JNIEnv* env, jobject, jbyteArray programImage) {
    const jsize length = env->GetArrayLength(programImage);
    std::vector<uint8_t> image(static_cast<size_t>(length));
    env->GetByteArrayRegion(programImage, 0, length, reinterpret_cast<jbyte*>(image.data()));
    return core().loadProgramImage(image);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_core_NativeApolloCore_resetScenario(JNIEnv*, jobject) {
    core().resetScenario();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_core_NativeApolloCore_pressKey(JNIEnv* env, jobject, jstring key) {
    const char* chars = env->GetStringUTFChars(key, nullptr);
    core().pressKey(chars);
    env->ReleaseStringUTFChars(key, chars);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_core_NativeApolloCore_stepSimulation(JNIEnv*, jobject, jdouble deltaSeconds) {
    core().stepSimulation(deltaSeconds);
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_myapplication_core_NativeApolloCore_getSnapshot(JNIEnv* env, jobject) {
    return toSnapshot(env, core().getSnapshot());
}
