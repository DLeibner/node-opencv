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

    interface Mat {
        rows: number;
        cols: number;
        data: Buffer;
        size: {
            width: number;
            height: number;
        }
        matchTemplate(template: Mat, method: number): Mat;
        matchTemplateAsync(template: Mat, method: number): Promise<Mat>;
        minMaxLocAsync(): Promise<MinMaxLocResult>;
        release(): void;
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
