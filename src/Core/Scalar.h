#pragma once
#include <napi.h>
#include <opencv2/opencv.hpp>

class Scalar : public Napi::ObjectWrap<Scalar> {
public:
    static Napi::Object init(Napi::Env env, Napi::Object exports);
    Scalar(const Napi::CallbackInfo& info);
    static Napi::FunctionReference constructor;

    cv::Scalar scalar;
};
