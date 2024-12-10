/// <reference types="node" />

declare namespace cv {
    interface Point {
        x: number;
        y: number;
    }

    interface MinMaxLocResult {
        minVal: number;
        maxVal: number;
        minLoc: Point;
        maxLoc: Point;
    }

    class Mat {
        rows: number;
        cols: number;
        data: Buffer;
        size: {
            width: number;
            height: number;
        };

        constructor();

        type(): number;
        matchTemplate(template: Mat, method: number): Mat;
        matchTemplateAsync(template: Mat, method: number): Promise<Mat>;
        minMaxLocAsync(): Promise<MinMaxLocResult>;
        release(): void;
        convertTo(outputMat: Mat, rtype: number, alpha: number, beta: number): void;
        cvtColor(destMat: Mat, code: number): void;
        roi(rect: Rect): Mat;
    };

    class Rect {
        constructor(x: number, y: number, width: number, height: number);
        x: number;
        y: number;
        width: number;
        height: number;
    }

    class Point {
        constructor(x: number, y: number);
        x: number;
        y: number;
    }

    class Scalar {
        constructor(v0: number, v1: number, v2: number, v3: number);
    }

    // constants
    const TM_CCOEFF_NORMED: number;
    const TM_CCORR_NORMED: number;
    const TM_SQDIFF_NORMED: number;
    const TM_CCOEFF: number;
    const TM_CCORR: number;
    const TM_SQDIFF: number;

    const IMREAD_COLOR: number;
    const IMREAD_GRAYSCALE: number;
    const IMREAD_UNCHANGED: number;
    const IMREAD_ANYDEPTH: number;
    const IMREAD_ANYCOLOR: number;

    const CV_8S: number;
    const CV_8U: number;
    const CV_8UC1: number;
    const CV_8UC3: number;
    const CV_8UC4: number;
    const CV_16S: number;
    const CV_32S: number;

    const COLOR_GRAY2RGBA: number;
    const COLOR_RGB2RGBA: number;

    // static methods
    function imdecodeAsync(buffer: Buffer, flag?: number): Promise<Mat>;
    function imreadAsync(filename: string, flag?: number): Promise<Mat>;
    function imdecode(buffer: Buffer, flag?: number): Mat;
    function imread(filename: string, flag?: number): Mat;
}

export = cv;
