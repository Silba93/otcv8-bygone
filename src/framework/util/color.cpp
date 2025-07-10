/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "color.h"
#include <cmath>

// NOTE: AABBGGRR order
const Color Color::alpha      = 0x00000000U;
const Color Color::white      = 0xffffffff;
const Color Color::black      = 0xff000000;
const Color Color::red        = 0xff0000ff;
const Color Color::darkRed    = 0xff000080;
const Color Color::green      = 0xff00ff00;
const Color Color::darkGreen  = 0xff008000;
const Color Color::blue       = 0xffff0000;
const Color Color::darkBlue   = 0xff800000;
const Color Color::pink       = 0xffff00ff;
const Color Color::darkPink   = 0xff800080;
const Color Color::yellow     = 0xff00ffff;
const Color Color::darkYellow = 0xff008080;
const Color Color::teal       = 0xffffff00;
const Color Color::darkTeal   = 0xff808000;
const Color Color::gray       = 0xffa0a0a0;
const Color Color::darkGray   = 0xff808080;
const Color Color::lightGray  = 0xffc0c0c0;
const Color Color::orange     = 0xff008cff;

Color::Color(const std::string& coltext)
{
    std::stringstream ss(coltext);
    ss >> *this;
}

void Color::getSaturationAndIntensity(int row, float &saturation, float &intensity) {
    // Pastel rows: light and low-saturation to high-saturation
    static const float saturationValues[] = { 0.20f, 0.30f, 0.40f, 0.60f, 0.80f, 1.0f, 1.0f };
    static const float intensityValues[]  = { 1.0f, 1.0f, 0.95f, 0.90f, 0.85f, 1.0f, 0.75f };

    if (row >= 0 && row < 7) {
        saturation = saturationValues[row];
        intensity = intensityValues[row];
    } else {
        saturation = 1.0f;
        intensity = 0.5f;
    }
}


void Color::HSVToRGB(float hue, float saturation, float intensity, float &r, float &g, float &b) {
    if (saturation == 0.0f) {
        r = g = b = intensity; // Grayscale
    } else {
        float sector = hue * 6.0f; // Which sector of the color wheel
        int i = static_cast<int>(sector);
        float f = sector - i;
        float p = intensity * (1.0f - saturation);
        float q = intensity * (1.0f - f * saturation);
        float t = intensity * (1.0f - (1.0f - f) * saturation);

        switch (i) {
            case 0: r = intensity; g = t; b = p; break;
            case 1: r = q; g = intensity; b = p; break;
            case 2: r = p; g = intensity; b = t; break;
            case 3: r = p; g = q; b = intensity; break;
            case 4: r = t; g = p; b = intensity; break;
            case 5: r = intensity; g = p; b = q; break;
        }
    }
}

Color Color::getOutfitColor(int color) {
    if (color < 0) color = 0;
    if (color > 255) color = 255;

    const int stepsPerColumn = 32;
    int column = color % stepsPerColumn;
    int row = color / stepsPerColumn;

    float hue = column / 32.0f; // Full hue cycle over 32 steps

    // Define saturation and intensity based on row
    float saturation, intensity;
    getSaturationAndIntensity(row, saturation, intensity);

    // Handle greyscale column
    if (column == 0) {
        int grayValue = 255 - (row * (255 / 7));
        return Color(grayValue, grayValue, grayValue);
    }

    // Convert from HSV to RGB
    float r = 0, g = 0, b = 0;
    HSVToRGB(hue, saturation, intensity, r, g, b);

    return Color(
        static_cast<int>(r * 255),
        static_cast<int>(g * 255),
        static_cast<int>(b * 255)
    );
}

std::string Color::toHex()
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}