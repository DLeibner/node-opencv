#pragma once
#include <napi.h>
#include <opencv2/opencv.hpp>

class Mat : public Napi::ObjectWrap<Mat> {
public:
    static Napi::Object init(Napi::Env env, Napi::Object exports);
    Mat(const Napi::CallbackInfo& info);
    ~Mat();

    cv::Mat mat;
    static Napi::Value imread(const Napi::CallbackInfo& info);
    static Napi::Value imdecode(const Napi::CallbackInfo& info);
    static Napi::Value imdecodeAsync(const Napi::CallbackInfo& info);
    static Napi::Value imreadAsync(const Napi::CallbackInfo& info);

    Napi::Value getCols(const Napi::CallbackInfo& info);
    Napi::Value getRows(const Napi::CallbackInfo& info);
    Napi::Value getData(const Napi::CallbackInfo& info);
    Napi::Value getSize(const Napi::CallbackInfo& info);

    Napi::Value matchTemplate(const Napi::CallbackInfo& info);
    Napi::Value matchTemplateAsync(const Napi::CallbackInfo& info);
    Napi::Value minMaxLocAsync(const Napi::CallbackInfo& info);
    Napi::Value release(const Napi::CallbackInfo& info);
    Napi::Value getType(const Napi::CallbackInfo& info);
    Napi::Value convertTo(const Napi::CallbackInfo& info);

protected:
    static Napi::FunctionReference constructor;
};
