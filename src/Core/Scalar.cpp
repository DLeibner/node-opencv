#include "Scalar.h"

Napi::FunctionReference Scalar::constructor;

Napi::Object Scalar::init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Scalar", {
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Scalar", func);
    return exports;
}

Scalar::Scalar(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Scalar>(info) {
    if (info.Length() < 4) {
        Napi::TypeError::New(info.Env(), "Expected 4 arguments").ThrowAsJavaScriptException();
        return;
    }

    scalar = cv::Scalar(
        info[0].As<Napi::Number>().DoubleValue(),
        info[1].As<Napi::Number>().DoubleValue(),
        info[2].As<Napi::Number>().DoubleValue(),
        info[3].As<Napi::Number>().DoubleValue()
    );
}
