#include "mat.h"
#include "async.h"

class

Napi::FunctionReference Mat::constructor;

Napi::Object Mat::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Mat", {
        InstanceMethod("matchTemplateAsync", &Mat::MatchTemplateAsync),
        InstanceMethod("matchTemplate", &Mat::MatchTemplate),
        InstanceMethod("minMaxLocAsync", &Mat::MinMaxLocAsync),
        InstanceMethod("release", &Mat::Release),
        StaticMethod("imdecodeAsync", &Mat::ImdecodeAsync),
        StaticMethod("imdecode", &Mat::Imdecode),
        StaticMethod("imread", &Mat::Imread),
        StaticMethod("imreadAsync", &Mat::ImreadAsync),

        InstanceAccessor<&Mat::GetCols>("cols"),
        InstanceAccessor<&Mat::GetRows>("rows"),
        InstanceAccessor<&Mat::GetData>("data"),
        InstanceAccessor<&Mat::GetSize>("size")
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

Napi::Value Mat::Imdecode(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        if (info.Length() < 1) {
            throw std::runtime_error("Buffer argument required");
        }
        if (!info[0].IsBuffer()) {
            throw std::runtime_error("Buffer argument must be a Buffer");
        }
        Napi::Buffer<uchar> buffer = info[0].As<Napi::Buffer<uchar>>();
        std::vector<uchar> buffer_data(buffer.Data(), buffer.Data() + buffer.Length());

        cv::Mat result = cv::imdecode(buffer_data, cv::IMREAD_COLOR);

        if (result.empty()) {
            throw std::runtime_error("Failed to decode image");
        }
        auto mat = Mat::constructor.New({});
        Mat* unwrapped = Mat::Unwrap(mat);
        unwrapped->mat = std::move(result);
        return mat;
    } catch (const std::exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
    }
}



Napi::Value Mat::ImdecodeAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        if (info.Length() < 1) {
            throw std::runtime_error("Buffer argument required");
        }
        if (!info[0].IsBuffer()) {
            throw std::runtime_error("Buffer argument must be a Buffer");
        }
        if (info.Length() > 1 && !info[1].IsNumber()) {
            throw std::runtime_error("Flags argument must be a number");
        }

        Napi::Buffer<uchar> buffer = info[0].As<Napi::Buffer<uchar>>();
        std::vector<uchar> buffer_data(buffer.Data(), buffer.Data() + buffer.Length());

        int flags = cv::IMREAD_COLOR;
        if (info.Length() > 1 && info[1].IsNumber()) {
            flags = info[1].As<Napi::Number>().Int32Value();
        }

        return AsyncWorker<cv::Mat>::Execute(
            env,
            [buffer_data = std::move(buffer_data), flags]() {
                cv::Mat result = cv::imdecode(buffer_data, flags);
                if (result.empty()) {
                    throw std::runtime_error("Failed to decode image");
                }
                return result;
            },
            [](Napi::Env env, const cv::Mat& result) {
                auto mat = Mat::constructor.New({});
                Mat* unwrapped = Mat::Unwrap(mat);
                unwrapped->mat = std::move(result);
                return mat;
            }
        );
    } catch (const std::exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
    }
}

Napi::Value Mat::Imread(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (info.Length() < 1) {
            throw std::runtime_error("Filename argument required");
        }
        if (!info[0].IsString()) {
            throw std::runtime_error("Filename argument must be a string");
        }
        std::string filename = info[0].As<Napi::String>().Utf8Value();
        int flags = cv::IMREAD_COLOR;

        if (info.Length() > 1 && info[1].IsNumber()) {
            flags = info[1].As<Napi::Number>().Int32Value();
        }

        cv::Mat result = cv::imread(filename, flags);
        if (result.empty()) {
            throw std::runtime_error("Failed to load image Path: " + filename);
        }
        auto mat = Mat::constructor.New({});
        Mat* unwrapped = Mat::Unwrap(mat);
        unwrapped->mat = std::move(result);
        return mat;
        return env.Null();
    } catch (const std::exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
    }
}

Napi::Value Mat::ImreadAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {

        if (info.Length() < 1) {
            throw std::runtime_error("Filename argument required");
        }
        if (!info[0].IsString()) {
            throw std::runtime_error("Filename argument must be a string");
        }
        if (info.Length() > 1 && !info[1].IsNumber()) {
            throw std::runtime_error("Flags argument must be a number");
        }

        std::string filename = info[0].As<Napi::String>().Utf8Value();
        int flags = cv::IMREAD_COLOR;

        if (info.Length() > 1 && info[1].IsNumber()) {
            flags = info[1].As<Napi::Number>().Int32Value();
        }

        return AsyncWorker<cv::Mat>::Execute(
            env,
            [filename, flags]() {
                cv::Mat result = cv::imread(filename, flags);
                if (result.empty()) {
                    throw std::runtime_error("Failed to load image: " + filename);
                }
                return result;
            },
            [](Napi::Env env, const cv::Mat& result) {
                auto mat = Mat::constructor.New({});
                Mat* unwrapped = Mat::Unwrap(mat);
                unwrapped->mat = result.clone();
                return mat;
            }
        );
    } catch (const std::exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
    }
}

Napi::Value Mat::MatchTemplateAsync(const Napi::CallbackInfo& info) {
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

Napi::Value Mat::MatchTemplate(const Napi::CallbackInfo& info) {
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

Napi::Value Mat::Release(const Napi::CallbackInfo& info) {
    mat.release();
    return info.This();
}

struct MinMaxResult {
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
};

Napi::Value Mat::MinMaxLocAsync(const Napi::CallbackInfo& info) {
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

Napi::Value Mat::GetCols(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), mat.cols);
}

Napi::Value Mat::GetRows(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), mat.rows);
}

Napi::Value Mat::GetSize(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto size = mat.size();
    auto obj = Napi::Object::New(env);
    obj.Set("width", Napi::Number::New(env, size.width));
    obj.Set("height", Napi::Number::New(env, size.height));
    return obj;
}


Napi::Value Mat::GetData(const Napi::CallbackInfo& info) {
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
