#pragma once
#include <napi.h>
#include <opencv2/opencv.hpp>

class Point : public Napi::ObjectWrap<Point> {
public:
    static Napi::Object init(Napi::Env env, Napi::Object exports);
    Point(const Napi::CallbackInfo& info);

    cv::Point point;

    Napi::Value getX(const Napi::CallbackInfo& info);
    Napi::Value getY(const Napi::CallbackInfo& info);

    static Napi::FunctionReference constructor;
};
