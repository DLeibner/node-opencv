#include "ImageUtils.h"
#include "Mat.h"
#include <AsyncWorker.h>

Napi::Object ImageUtils::init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    exports.Set("imdecode", Napi::Function::New(env, &ImageUtils::imdecode));
    exports.Set("imdecodeAsync", Napi::Function::New(env, &ImageUtils::imdecodeAsync));
    exports.Set("imread", Napi::Function::New(env, &ImageUtils::imread));
    exports.Set("imreadAsync", Napi::Function::New(env, &ImageUtils::imreadAsync));

    return exports;
}

Napi::Value ImageUtils::imdecode(const Napi::CallbackInfo& info) {
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

Napi::Value ImageUtils::imdecodeAsync(const Napi::CallbackInfo& info) {
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

Napi::Value ImageUtils::imread(const Napi::CallbackInfo& info) {
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
    } catch (const std::exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
    }
}

Napi::Value ImageUtils::imreadAsync(const Napi::CallbackInfo& info) {
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
