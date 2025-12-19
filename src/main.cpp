#include <Arduino.h>
#include <SD.h>

#include "LGFX_ESP32_8048S050C.hpp"
#include <LovyanGFX.hpp>
#include <ScreenShot.hpp>

#include "Flock.hpp"

LGFX gfx;
LGFX_Sprite simSprite(&gfx);
LGFX_Sprite uiSprite(&gfx);
bool uiDirty = true;

static Flock swarm;

struct Slider
{
    int x, y, w, h;
    float minVal, maxVal;
    float value;
    const char *label;
};

Slider sepSlider = {20, 160, 210, 14, 0.0f, 10.0f, swarm.sepWeight, "Separation weight"};
Slider aliSlider = {20, 220, 210, 14, 0.0f, 10.0f, swarm.aliWeight, "Alignment weight"};
Slider cohSlider = {20, 280, 210, 14, 0.0f, 10.0f, swarm.cohWeight, "Cohesion weight"};

struct Button
{
    int x, y, w, h;
    const char *label;
};

Button clearButton = {20, 330, 280, 50, "Clear all Boids"};
Button resetButton = {20, 400, 280, 50, "Restart simulation"};

void drawButton(LovyanGFX &dest, Button &b)
{
    dest.fillRect(b.x, b.y, b.w, b.h, TFT_LIGHTGREY);
    dest.setTextColor(TFT_BLACK);
    dest.setTextDatum(CC_DATUM);
    dest.drawCenterString(b.label, b.x + (b.w / 2), b.y + (b.h / 2) - (DejaVu18.yAdvance / 2), &DejaVu18);
}

void drawSlider(LGFX_Sprite &dest, Slider &s)
{
    // Label
    dest.setTextDatum(TL_DATUM);
    dest.setTextColor(TFT_WHITE);
    dest.drawString(s.label, s.x, s.y - 25, &DejaVu18);

    // Track
    dest.fillRect(s.x, s.y, s.w, s.h, TFT_LIGHTGREY);

    // Filled part
    int fillW = (int)((s.value - s.minVal) / (s.maxVal - s.minVal) * s.w);
    dest.fillRect(s.x, s.y, fillW, s.h, TFT_GREEN);

    // Knob
    int knobX = s.x + fillW;
    dest.fillRect(knobX - 4, s.y - 4, 8, s.h + 8, TFT_WHITE);

    // Value text
    char valStr[12];
    snprintf(valStr, sizeof(valStr), "%.2f", s.value);
    dest.drawString(valStr, s.x + s.w + 16, s.y - 2, &DejaVu18);
}

bool updateSliderFromTouch(Slider &s, int tx, int ty, float &valToWrite)
{
    float old = s.value;
    if (tx >= s.x && tx <= s.x + s.w &&
        ty >= s.y - 10 && ty <= s.y + s.h + 10)
    {
        float t = (float)(tx - s.x) / s.w;
        t = constrain(t, 0.0f, 1.0f);
        s.value = s.minVal + t * (s.maxVal - s.minVal);
        valToWrite = s.value;
    }
    return s.value != old;
}

void drawAllUI()
{
    uiSprite.clear(TFT_DARKGREY);
    uiSprite.setTextDatum(TL_DATUM);
    uiSprite.setTextColor(TFT_WHITE);
    uiSprite.drawString("Flocking", 20, 40, &DejaVu40);
    char buf[32];
    snprintf(buf, sizeof(buf), "Boids: %i", swarm.boids.size());
    uiSprite.drawString(buf, 20, 100, &DejaVu18);
    drawSlider(uiSprite, sepSlider);
    drawSlider(uiSprite, aliSlider);
    drawSlider(uiSprite, cohSlider);
    drawButton(uiSprite, clearButton);
    drawButton(uiSprite, resetButton);
    uiSprite.pushSprite(480, 0);
}

inline bool hitButton(const Button &b, int x, int y)
{
    return (x >= b.x && x <= b.x + b.w &&
            y >= b.y && y <= b.y + b.h);
}

void takeScreenShot()
{
    ScreenShot sShot;
    String error;
    bool success = sShot.saveBMP("/screenshot.bmp", gfx, SD, error);
    gfx.setTextDatum(CC_DATUM);
    gfx.fillRect(480, 0, 320, 40, TFT_BLACK);
    gfx.drawCenterString(success ? "Screenshot saved" : error, 480 + 160, 20 - (DejaVu12.yAdvance / 2), &DejaVu12);
}

void handleTouch(lgfx::touch_point_t &tp)
{
    bool changed = false;
    if (tp.x < 480)
    {
        swarm.addBoid(tp.x, tp.y);
        changed = true;
    }
    else
    {
        int uiTx = tp.x - 480; // UI sprite X offset
        int uiTy = tp.y;

        changed |= updateSliderFromTouch(sepSlider, uiTx, uiTy, swarm.sepWeight);
        changed |= updateSliderFromTouch(aliSlider, uiTx, uiTy, swarm.aliWeight);
        changed |= updateSliderFromTouch(cohSlider, uiTx, uiTy, swarm.cohWeight);

        if (hitButton(clearButton, uiTx, uiTy))
        {
            swarm.clear();
            changed = true;
        }
        else if (hitButton(resetButton, uiTx, uiTy))
        {
            swarm.clear();
            for (int i = 0; i < 40; i++)
                swarm.addBoid(random(0, simSprite.width()),
                              random(0, simSprite.height()));
            changed = true;
        }
        else if (uiTy < 40)
            takeScreenShot();
    }

    if (changed)
        uiDirty = true;
}

void setup()
{
    Serial.begin(115200);

    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    bool sdStarted = SD.begin(SD_SS);

    gfx.setRotation(0);
    gfx.setBrightness(110);
    if (!gfx.begin())
    {
        log_e("Failed to init gfx!");
        while (true)
            delay(100);
    }
/*
    if (!sdStarted)
    {
        gfx.drawCenterString("Could not mount SD. Tap to continue.", gfx.width() / 2, gfx.height() / 2, &DejaVu18);

        lgfx::touch_point_t tp;
        while (!gfx.getTouch(&tp))
            delay(10);
    }
*/
    simSprite.setPsram(true);
    simSprite.createSprite(480, 480);

    if (!simSprite.getBuffer())
    {
        log_e("Could not allocate simulation frame buffer. system halted.");
        while (1)
            delay(100);
    }

    uiSprite.setPsram(true);
    uiSprite.createSprite(320, 480);

    if (!uiSprite.getBuffer())
    {
        log_e("Could not allocate ui frame buffer. system halted.");
        while (1)
            delay(100);
    }

    for (int i = 0; i < 40; i++)
        swarm.addBoid(random(0, simSprite.width()), random(0, simSprite.height()));

    log_i("gfx initialized");
}

void loop()
{
    simSprite.clear(lgfx::color565(0, 0, 0));
    swarm.run(simSprite.width(), simSprite.height(), simSprite);
    simSprite.pushSprite(0, 0);

    lgfx::touch_point_t tp;
    bool touched = gfx.getTouch(&tp);

    if (touched)
        handleTouch(tp);

    if (uiDirty)
    {
        drawAllUI();
        uiDirty = false;
    }
}
