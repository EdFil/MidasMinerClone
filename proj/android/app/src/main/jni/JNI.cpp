#include <jni.h>
#include <string>

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    // Get jclass with env->FindClass.
    // Register methods with env->RegisterNatives.

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLInputConnection_nativeSetComposingText(JNIEnv* env,
                                                                           jobject instance,
                                                                           jstring text_,
                                                                           jint newCursorPosition) {
    const char* text = env->GetStringUTFChars(text_, 0);
    env->ReleaseStringUTFChars(text_, text);
}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLInputConnection_nativeCommitText(JNIEnv *env, jobject instance,
                                                                     jstring text_,
                                                                     jint newCursorPosition) {
    const char *text = env->GetStringUTFChars(text_, 0);
    env->ReleaseStringUTFChars(text_, text);
}

JNIEXPORT jstring JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_nativeGetHint(JNIEnv *env, jclass type,
                                                           jstring name_) {
    const char *name = env->GetStringUTFChars(name_, 0);
    env->ReleaseStringUTFChars(name_, name);
    return env->NewStringUTF("");
}

JNIEXPORT jint JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_nativeRemoveJoystick(JNIEnv *env, jclass type,
                                                                  jint device_id) {
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_nativeAddJoystick(JNIEnv *env, jclass type,
                                                               jint device_id, jstring name_,
                                                               jint is_accelerometer, jint nbuttons,
                                                               jint naxes, jint nhats,
                                                               jint nballs) {
    const char *name = env->GetStringUTFChars(name_, 0);
    env->ReleaseStringUTFChars(name_, name);
    return 0;
}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeSurfaceDestroyed(JNIEnv *env, jclass type) {

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeSurfaceChanged(JNIEnv *env, jclass type) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeAccel(JNIEnv *env, jclass type, jfloat x,
                                                           jfloat y, jfloat z) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeTouch(JNIEnv *env, jclass type,
                                                           jint touchDevId, jint pointerFingerId,
                                                           jint action, jfloat x, jfloat y,
                                                           jfloat p) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeMouse(JNIEnv *env, jclass type, jint button,
                                                           jint action, jfloat x, jfloat y) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeKeyboardFocusLost(JNIEnv *env, jclass type) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeKeyUp(JNIEnv *env, jclass type, jint keycode) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeKeyDown(JNIEnv *env, jclass type,
                                                             jint keycode) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeHat(JNIEnv *env, jclass type, jint device_id,
                                                         jint hat_id, jint x, jint y) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeJoy(JNIEnv *env, jclass type, jint device_id,
                                                         jint axis, jfloat value) {

    // TODO

}

JNIEXPORT jint JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativePadUp(JNIEnv *env, jclass type, jint device_id,
                                                           jint keycode) {
    return 0;
    // TODO

}

JNIEXPORT jint JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativePadDown(JNIEnv *env, jclass type,
                                                             jint device_id, jint keycode) {
    return 0;
    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeResize(JNIEnv *env, jclass type, jint x,
                                                            jint y, jint format, jfloat rate) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_onNativeDropFile(JNIEnv *env, jclass type,
                                                              jstring filename_) {
    const char *filename = env->GetStringUTFChars(filename_, 0);
    env->ReleaseStringUTFChars(filename_, filename);
}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_nativeResume(JNIEnv *env, jclass type) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_nativePause(JNIEnv *env, jclass type) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_nativeQuit(JNIEnv *env, jclass type) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_nativeLowMemory(JNIEnv *env, jclass type) {

    // TODO

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_edfil_conceptv2_SDLActivity_nativeInit(JNIEnv *env, jclass type,
                                                        jobject arguments) {

    // TODO
    return 0;
}
