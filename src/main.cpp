#include <napi.h>
#include "Core/ImageUtils.h"
#include "Core/Mat.h"
#include "Core/Rect.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "TM_SQDIFF"), Napi::Number::New(env, cv::TM_SQDIFF));
    exports.Set(Napi::String::New(env, "TM_SQDIFF_NORMED"), Napi::Number::New(env, cv::TM_SQDIFF_NORMED));
    exports.Set(Napi::String::New(env, "TM_CCORR"), Napi::Number::New(env, cv::TM_CCORR));
    exports.Set(Napi::String::New(env, "TM_CCORR_NORMED"), Napi::Number::New(env, cv::TM_CCORR_NORMED));
    exports.Set(Napi::String::New(env, "TM_CCOEFF"), Napi::Number::New(env, cv::TM_CCOEFF));
    exports.Set(Napi::String::New(env, "TM_CCOEFF_NORMED"), Napi::Number::New(env, cv::TM_CCOEFF_NORMED));

    exports.Set(Napi::String::New(env, "IMREAD_COLOR"), Napi::Number::New(env, cv::IMREAD_COLOR));
    exports.Set(Napi::String::New(env, "IMREAD_GRAYSCALE"), Napi::Number::New(env, cv::IMREAD_GRAYSCALE));
    exports.Set(Napi::String::New(env, "IMREAD_UNCHANGED"), Napi::Number::New(env, cv::IMREAD_UNCHANGED));
    exports.Set(Napi::String::New(env, "IMREAD_ANYDEPTH"), Napi::Number::New(env, cv::IMREAD_ANYDEPTH));
    exports.Set(Napi::String::New(env, "IMREAD_ANYCOLOR"), Napi::Number::New(env, cv::IMREAD_ANYCOLOR));

    exports.Set(Napi::String::New(env, "CV_8S"), Napi::Number::New(env, 1));
    exports.Set(Napi::String::New(env, "CV_8U"), Napi::Number::New(env, 0));
    exports.Set(Napi::String::New(env, "CV_8UC1"), Napi::Number::New(env, 0));
    exports.Set(Napi::String::New(env, "CV_8UC3"), Napi::Number::New(env, 16));
    exports.Set(Napi::String::New(env, "CV_8UC4"), Napi::Number::New(env, 24));
    exports.Set(Napi::String::New(env, "CV_16S"), Napi::Number::New(env, 3));
    exports.Set(Napi::String::New(env, "CV_32S"), Napi::Number::New(env, 4));

    exports.Set(Napi::String::New(env, "COLOR_GRAY2RGBA"), Napi::Number::New(env, cv::COLOR_GRAY2RGBA));
    exports.Set(Napi::String::New(env, "COLOR_RGB2RGBA"), Napi::Number::New(env, cv::COLOR_RGB2RGBA));

    exports = Mat::init(env, exports);
    exports = ImageUtils::init(env, exports);
    exports = Rect::init(env, exports);

    return exports;
}

NODE_API_MODULE(opencv_addon, Init)
