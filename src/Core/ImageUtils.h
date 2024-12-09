#pragma once
#include <napi.h>
#include <opencv2/opencv.hpp>

class ImageUtils : public Napi::ObjectWrap<ImageUtils> {
public:
    static Napi::Object init(Napi::Env env, Napi::Object exports);

    static Napi::Value imdecode(const Napi::CallbackInfo& info);
    static Napi::Value imdecodeAsync(const Napi::CallbackInfo& info);
    static Napi::Value imread(const Napi::CallbackInfo& info);
    static Napi::Value imreadAsync(const Napi::CallbackInfo& info);
};
