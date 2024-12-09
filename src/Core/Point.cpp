#include "Point.h"

Napi::FunctionReference Point::constructor;

Napi::Object Point::init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Point", {
        InstanceAccessor<&Point::getX>("x"),
        InstanceAccessor<&Point::getY>("y"),
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Point", func);
    return exports;
}

Point::Point(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Point>(info) {
        Napi::Env env = info.Env();
        Napi::HandleScope scope(env);

        if (info.Length() < 2) {
            Napi::TypeError::New(env, "Expected 2 arguments: x, y").ThrowAsJavaScriptException();
            return;
        }

        point.x = info[0].As<Napi::Number>().Int32Value();
        point.y = info[1].As<Napi::Number>().Int32Value();
}

Napi::Value Point::getX(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), point.x);
}

Napi::Value Point::getY(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), point.y);
}
