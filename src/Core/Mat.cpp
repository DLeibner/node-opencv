#include "Mat.h"
#include "../Threading/AsyncWorker.h"
#include "Rect.h"
#include "Point.h"
#include "Scalar.h"

Napi::FunctionReference Mat::constructor;

Napi::Object Mat::init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Mat", {
        InstanceMethod("matchTemplateAsync", &Mat::matchTemplateAsync),
        InstanceMethod("matchTemplate", &Mat::matchTemplate),
        InstanceMethod("minMaxLocAsync", &Mat::minMaxLocAsync),
        InstanceMethod("release", &Mat::release),
        InstanceMethod("convertTo", &Mat::convertTo),
        InstanceMethod("type", &Mat::getType),
        InstanceMethod("cvtColor", &Mat::cvtColor),
        InstanceMethod("roi", &Mat::roi),
        InstanceMethod("drawRectangle", &Mat::drawRectangle),

        InstanceAccessor<&Mat::getCols>("cols"),
        InstanceAccessor<&Mat::getRows>("rows"),
        InstanceAccessor<&Mat::getData>("data"),
        InstanceAccessor<&Mat::getSize>("size"),
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Mat", func);
    return exports;
}

Mat::Mat(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Mat>(info) {
}

Mat::~Mat() {
    if (!mat.empty()) {
        mat.release();
    }
}

Napi::Value Mat::matchTemplateAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    if (info.Length() < 2) {
        deferred.Reject(Napi::TypeError::New(env, "Expected 2 arguments").Value());
        return deferred.Promise();
    }

    if (!info[0].As<Napi::Object>().InstanceOf(Mat::constructor.Value())) {
        deferred.Reject(Napi::TypeError::New(env, "First argument must be a Mat instance").Value());
        return deferred.Promise();
    }

    if (!info[1].IsNumber()) {
        deferred.Reject(Napi::TypeError::New(env, "Second argument must be a number").Value());
        return deferred.Promise();
    }

    Mat* templ = Napi::ObjectWrap<Mat>::Unwrap(info[0].As<Napi::Object>());
    int method = info[1].As<Napi::Number>().Int32Value();

    auto sourceMat = this->mat;
    auto templateMat = templ->mat;

    return AsyncWorker<cv::Mat>::Execute(
        env,
        [sourceMat, templateMat, method]() {
            cv::Mat result;
            cv::matchTemplate(sourceMat, templateMat, result, method);
            return result;
        },
        [](Napi::Env env, const cv::Mat& result) {
            auto mat = Mat::constructor.New({});
            Mat* unwrapped = Mat::Unwrap(mat);
            unwrapped->mat = result.clone();
            return mat;
        }
    );
}

Napi::Value Mat::matchTemplate(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected 2 arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].As<Napi::Object>().InstanceOf(Mat::constructor.Value())) {
        Napi::TypeError::New(env, "First argument must be a Mat instance").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Second argument must be a number").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Mat* templ = Napi::ObjectWrap<Mat>::Unwrap(info[0].As<Napi::Object>());
    int method = info[1].As<Napi::Number>().Int32Value();

    cv::Mat result;
    cv::matchTemplate(mat, templ->mat, result, method);

    auto mat = Mat::constructor.New({});
    Mat* unwrapped = Mat::Unwrap(mat);
    unwrapped->mat = std::move(result);
    return mat;
}

Napi::Value Mat::release(const Napi::CallbackInfo& info) {
    mat.release();
    return info.This();
}

Napi::Value Mat::convertTo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected at least 2 arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].As<Napi::Object>().InstanceOf(Mat::constructor.Value())) {
        Napi::TypeError::New(env, "First argument must be a Mat instance").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Second argument must be a number").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Mat* outputMat = Napi::ObjectWrap<Mat>::Unwrap(info[0].As<Napi::Object>());
    int rtype = info[1].As<Napi::Number>().Int32Value();

    double alpha = 1.0;
    double beta = 0.0;

    if (info.Length() > 2 && info[2].IsNumber()) {
        alpha = info[2].As<Napi::Number>().DoubleValue();
    }

        if (info.Length() > 3 && info[3].IsNumber()) {
        beta = info[3].As<Napi::Number>().DoubleValue();
    }

    try {
            mat.convertTo(outputMat->mat, rtype, alpha, beta);
        } catch (const std::exception& e) {
            Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
            return env.Null();
        }

    return env.Null();
}

struct MinMaxResult {
    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;
};

Napi::Value Mat::minMaxLocAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto self = this;

    return AsyncWorker<MinMaxResult>::Execute(
        env,
        [self]() {
            MinMaxResult result;
            cv::minMaxLoc(self->mat, &result.minVal, &result.maxVal, &result.minLoc, &result.maxLoc);
            return result;
        },
        [](Napi::Env env, const MinMaxResult& result) {
            auto obj = Napi::Object::New(env);
            obj.Set("minVal", Napi::Number::New(env, result.minVal));
            obj.Set("maxVal", Napi::Number::New(env, result.maxVal));

            auto minLocObj = Napi::Object::New(env);
            minLocObj.Set("x", Napi::Number::New(env, result.minLoc.x));
            minLocObj.Set("y", Napi::Number::New(env, result.minLoc.y));
            obj.Set("minLoc", minLocObj);

            auto maxLocObj = Napi::Object::New(env);
            maxLocObj.Set("x", Napi::Number::New(env, result.maxLoc.x));
            maxLocObj.Set("y", Napi::Number::New(env, result.maxLoc.y));
            obj.Set("maxLoc", maxLocObj);
            return obj;
        }
    );
}

Napi::Value Mat::getCols(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), mat.cols);
}

Napi::Value Mat::getRows(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), mat.rows);
}

Napi::Value Mat::getSize(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto size = mat.size();
    auto obj = Napi::Object::New(env);
    obj.Set("width", Napi::Number::New(env, size.width));
    obj.Set("height", Napi::Number::New(env, size.height));
    return obj;
}

Napi::Value Mat::getData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (mat.empty()) {
        return Napi::Buffer<uchar>::New(env, 0);
    }

    size_t dataSize = mat.total() * mat.elemSize();
    if (dataSize == 0) {
        return Napi::Buffer<uchar>::New(env, 0);
    }

    return Napi::Buffer<uchar>::New(
        env,
        mat.data,
        dataSize
    );
}

Napi::Value Mat::getType(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), mat.type());
}

Napi::Value Mat::cvtColor(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected 2 arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].As<Napi::Object>().InstanceOf(Mat::constructor.Value())) {
        Napi::TypeError::New(env, "First argument must be a Mat instance").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Second argument must be a number").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Mat* dest = Napi::ObjectWrap<Mat>::Unwrap(info[0].As<Napi::Object>());
    int code = info[1].As<Napi::Number>().Int32Value();

    cv::cvtColor(mat, dest->mat, code);

    return env.Undefined();
}

Napi::Value Mat::roi(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected 1 argument").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].As<Napi::Object>().InstanceOf(Rect::constructor.Value())) {
        Napi::TypeError::New(env, "First argument must be a Rect instance").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Rect* rect = Napi::ObjectWrap<Rect>::Unwrap(info[0].As<Napi::Object>());

    auto result = mat(rect->rect);

    auto mat = Mat::constructor.New({});
    Mat* unwrapped = Mat::Unwrap(mat);
    unwrapped->mat = std::move(result);
    return mat;
}

Napi::Value Mat::drawRectangle(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 4) {
        Napi::TypeError::New(env, "Expected 4 arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].As<Napi::Object>().InstanceOf(Point::constructor.Value())) {
        Napi::TypeError::New(env, "First argument must be a Point instance").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[1].As<Napi::Object>().InstanceOf(Point::constructor.Value())) {
        Napi::TypeError::New(env, "Second argument must be a Point instance").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[2].As<Napi::Object>().InstanceOf(Scalar::constructor.Value())) {
        Napi::TypeError::New(env, "Third argument must be a Scalar instance").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[3].As<Napi::Object>().IsNumber()) {
        Napi::TypeError::New(env, "Fourth argument must be a number").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Point* point1 = Napi::ObjectWrap<Point>::Unwrap(info[0].As<Napi::Object>());
    Point* point2 = Napi::ObjectWrap<Point>::Unwrap(info[1].As<Napi::Object>());
    Scalar* color = Napi::ObjectWrap<Scalar>::Unwrap(info[2].As<Napi::Object>());
    int thickness = info[3].As<Napi::Number>().Int32Value();

    cv::rectangle(mat, point1->point, point2->point, color->scalar, thickness);

    return env.Undefined();
}
