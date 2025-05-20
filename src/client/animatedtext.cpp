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

#include "animatedtext.h"
#include "map.h"
#include "game.h"
#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/graphics.h>
const double M_PI = 3.14159265358979323846;

AnimatedText::AnimatedText()
{
    m_cachedText.setFont(g_fonts.getFont("verdana-11px-rounded"));
    m_cachedText.setAlign(Fw::AlignLeft);
}

void AnimatedText::drawText(const Point& dest, const Rect& visibleRect)
{
    static float tf = Otc::ANIMATED_TEXT_DURATION;

    float t = m_animationTimer.ticksElapsed() / 1000.0f; // seconds

    PointF pos(dest.x, dest.y);
    pos += m_fanDirection * t;
    pos += PointF(m_offset.x, m_offset.y); // if you still use m_offset

    Size textSize = m_cachedText.getTextSize();
    Point drawPos(pos.x - textSize.width() / 2, pos.y);

    Rect rect(drawPos, textSize);
    if (visibleRect.contains(rect)) {
        float t0 = tf / 1.2f;
        Color color = m_color;
        float ticks = m_animationTimer.ticksElapsed();

        if (ticks > t0) {
            color.setAlpha((float)(1.0f - (ticks - t0) / (tf - t0)));
        }

        m_cachedText.draw(rect, color);
    }
}


void AnimatedText::onAppear()
{
    m_animationTimer.restart();

    // Choose these:
    float centerAngle = -90.0f;  // 90 = North
    float spread = -150.0f;      // 180 = east to west spread (half a circle)

    float minAngle = centerAngle - spread / 2.0f;
    float maxAngle = centerAngle + spread / 2.0f;

    // Random float 0..1
    float randFraction = std::rand() / (float)RAND_MAX;

    // Random angle within the spread
    float angle = minAngle + spread * randFraction;

    // Wrap angle between 0° and 360°
    if (angle < 0.0f)
        angle += 360.0f;
    else if (angle >= 360.0f)
        angle -= 360.0f;

    float radians = angle * M_PI / 180.0f;

    // Random speed between 40 and 60 px/sec
    float minSpeed = 50.0f;
    float maxSpeed = 80.0f;
    float speed = minSpeed + (maxSpeed - minSpeed) * randFraction;

    m_fanDirection.x = std::cos(radians) * speed;
    m_fanDirection.y = std::sin(radians) * speed;

    auto self = asAnimatedText();
    g_dispatcher.scheduleEvent([self]() { g_map.removeThing(self); }, Otc::ANIMATED_TEXT_DURATION);
}




void AnimatedText::setColor(int color)
{
    m_color = Color::from8bit(color);
}

void AnimatedText::setText(const std::string& text)
{
    m_cachedText.setText(text);
}

void AnimatedText::setFont(const std::string& fontName)
{
    m_cachedText.setFont(g_fonts.getFont(fontName));
}

bool AnimatedText::merge(const AnimatedTextPtr& other)
{
    if(other->getColor() != m_color)
        return false;

    if(other->getCachedText().getFont() != m_cachedText.getFont())
        return false;

    if(m_animationTimer.ticksElapsed() > Otc::ANIMATED_TEXT_DURATION / 2.5)
        return false;

    try {
        int number = stdext::safe_cast<int>(m_cachedText.getText());
        int otherNumber = stdext::safe_cast<int>(other->getCachedText().getText());
        m_cachedText.setText(std::to_string(number + otherNumber));
        return true;
    } catch(...) {}
    return false;
}
