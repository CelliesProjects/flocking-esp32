#pragma once
#include <vector>

#include "Boid.hpp"

class Flock
{
public:
    float sepWeight = 5.1f;
    float aliWeight = 1.55f;
    float cohWeight = 2.35f;

    std::vector<std::unique_ptr<Boid>> boids;

    Flock()
    {
        boids.reserve(128);
    }

    void run(int w, int h, LovyanGFX &gfx)
    {
        if (!boids.size())
        {
            gfx.setTextDatum(CC_DATUM);
            gfx.drawString("Tap to create Boids", gfx.width() / 2, gfx.height() / 2, &DejaVu24);
            return;
        }
        // Build a non-owning view (raw pointers)
        std::vector<Boid *> boidPtrs;
        boidPtrs.reserve(boids.size());

        for (auto &b : boids)
            boidPtrs.push_back(b.get());

        for (auto &b : boids)
            b->run(boidPtrs, sepWeight, aliWeight, cohWeight, w, h, gfx);
    }

    void addBoid(float x, float y)
    {
        boids.emplace_back(std::make_unique<Boid>(x, y));
    }

    void clear()
    {
        boids.clear();
    }
};
