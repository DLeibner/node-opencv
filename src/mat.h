#pragma once
#include <napi.h>
#include <opencv2/opencv.hpp>

class Mat : public Napi::ObjectWrap<Mat> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Mat(const Napi::CallbackInfo& info);
    ~Mat();

    cv::Mat mat;
    static Napi::Value Imread(const Napi::CallbackInfo& info);
    static Napi::Value Imdecode(const Napi::CallbackInfo& info);
    static Napi::Value ImdecodeAsync(const Napi::CallbackInfo& info);
    static Napi::Value ImreadAsync(const Napi::CallbackInfo& info);

    Napi::Value GetCols(const Napi::CallbackInfo& info);
    Napi::Value GetRows(const Napi::CallbackInfo& info);
    Napi::Value GetData(const Napi::CallbackInfo& info);
    Napi::Value GetSize(const Napi::CallbackInfo& info);
    Napi::Value MatchTemplate(const Napi::CallbackInfo& info);
    Napi::Value MatchTemplateAsync(const Napi::CallbackInfo& info);
    Napi::Value MinMaxLocAsync(const Napi::CallbackInfo& info);
    Napi::Value Release(const Napi::CallbackInfo& info);

protected:
    static Napi::FunctionReference constructor;
};
