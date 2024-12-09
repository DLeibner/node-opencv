#include "Rect.h"

Napi::FunctionReference Rect::constructor;

Napi::Object Rect::init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Rect", {
        InstanceAccessor<&Rect::getX>("x"),
        InstanceAccessor<&Rect::getY>("y"),
        InstanceAccessor<&Rect::getWidth>("width"),
        InstanceAccessor<&Rect::getHeight>("height"),
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Rect", func);
    return exports;
}

Rect::Rect(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Rect>(info) {
        Napi::Env env = info.Env();
        Napi::HandleScope scope(env);

        if (info.Length() < 4) {
            Napi::TypeError::New(env, "Expected 4 arguments: x, y, width, height").ThrowAsJavaScriptException();
            return;
        }

        rect.x = info[0].As<Napi::Number>().Int32Value();
        rect.y = info[1].As<Napi::Number>().Int32Value();
        rect.width = info[2].As<Napi::Number>().Int32Value();
        rect.height = info[3].As<Napi::Number>().Int32Value();
}

Napi::Value Rect::getX(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), rect.x);
}

Napi::Value Rect::getY(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), rect.y);
}

Napi::Value Rect::getWidth(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), rect.width);
}

Napi::Value Rect::getHeight(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), rect.height);
}
