#pragma once
#include <napi.h>
#include <opencv2/opencv.hpp>

class Rect : public Napi::ObjectWrap<Rect> {
public:
    static Napi::Object init(Napi::Env env, Napi::Object exports);
    Rect(const Napi::CallbackInfo& info);

    cv::Rect rect;

    Napi::Value getX(const Napi::CallbackInfo& info);
    Napi::Value getY(const Napi::CallbackInfo& info);
    Napi::Value getWidth(const Napi::CallbackInfo& info);
    Napi::Value getHeight(const Napi::CallbackInfo& info);

    static Napi::FunctionReference constructor;
};
