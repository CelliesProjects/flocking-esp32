#pragma once
#include <Arduino.h>
#include <LovyanGFX.hpp>

#include "Vec2.hpp"

class Boid
{
public:
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;

    float maxforce = 0.03f; // Steering force limit
    float maxspeed = 2.0f;  // Speed limit
    float size = 6.0f;      // Length of the drawn line

    Boid(float x, float y)
        : position(x, y)
    {
        // Random initial velocity
        velocity = Vec2(random(-100, 100) * 0.01f,
                        random(-100, 100) * 0.01f);
    }

    // ------------------------------------------------------------
    // Apply force (adds to acceleration)
    // ------------------------------------------------------------
    inline void applyForce(const Vec2 &force)
    {
        acceleration += force;
    }

    // ------------------------------------------------------------
    // Main per-frame update
    // ------------------------------------------------------------
    void update()
    {
        velocity += acceleration;
        velocity.limit(maxspeed);
        position += velocity;
        acceleration *= 0; // reset acceleration for next frame
    }

    // ------------------------------------------------------------
    // Borders wrap-around
    // ------------------------------------------------------------
    void borders(int w, int h)
    {
        if (position.x < 0)
            position.x = w;
        if (position.y < 0)
            position.y = h;
        if (position.x > w)
            position.x = 0;
        if (position.y > h)
            position.y = 0;
    }

    // ------------------------------------------------------------
    // Steering behaviors
    // ------------------------------------------------------------

    Vec2 separate(const std::vector<Boid *> &boids)
    {
        float desiredSeparation = 30.0f;
        Vec2 steer(0, 0);
        int count = 0;

        for (Boid *other : boids)
        {
            float d = position.dist(other->position);
            if (d > 0 && d < desiredSeparation)
            {
                Vec2 diff = position - other->position;
                diff.normalize();
                diff /= d; // inverse distance weight
                steer += diff;
                count++;
            }
        }

        if (count > 0)
            steer /= (float)count;

        if (steer.magSq() > 0)
        {
            steer.normalize();
            steer *= maxspeed;
            steer -= velocity;
            steer.limit(maxforce);
        }

        return steer;
    }

    Vec2 align(const std::vector<Boid *> &boids)
    {
        float neighborDist = 40.0f;
        Vec2 sum(0, 0);
        int count = 0;

        for (Boid *other : boids)
        {
            float d = position.dist(other->position);
            if (d > 0 && d < neighborDist)
            {
                sum += other->velocity;
                count++;
            }
        }

        if (count > 0)
        {
            sum /= (float)count;
            sum.normalize();
            sum *= maxspeed;

            Vec2 steer = sum - velocity;
            steer.limit(maxforce);
            return steer;
        }

        return Vec2(0, 0);
    }

    Vec2 cohesion(const std::vector<Boid *> &boids)
    {
        float neighborDist = 40.0f;
        Vec2 sum(0, 0);
        int count = 0;

        for (Boid *other : boids)
        {
            float d = position.dist(other->position);
            if (d > 0 && d < neighborDist)
            {
                sum += other->position;
                count++;
            }
        }

        if (count > 0)
        {
            sum /= (float)count;
            return seek(sum);
        }

        return Vec2(0, 0);
    }

    // ------------------------------------------------------------
    // Seek helper used by cohesion()
    // ------------------------------------------------------------
    Vec2 seek(const Vec2 &target)
    {
        Vec2 desired = target - position;
        desired.normalize();
        desired *= maxspeed;

        Vec2 steer = desired - velocity;
        steer.limit(maxforce);
        return steer;
    }

    // ------------------------------------------------------------
    // Render: simple line pointing in direction of movement
    // ------------------------------------------------------------

    void render(LovyanGFX &gfx)
    {
        float angle = velocity.heading();

        int x1 = (int)position.x;
        int y1 = (int)position.y;

        int x2 = (int)(position.x + cosf(angle) * size);
        int y2 = (int)(position.y + sinf(angle) * size);

        gfx.drawLine(x1, y1, x2, y2, gfx.color565(255, 255, 255));
    }

    void run(const std::vector<Boid *> &boids,
             float sepWeight,
             float aliWeight,
             float cohWeight,
             int w,
             int h,
             LovyanGFX &gfx)
    {
        // --- steering forces ---
        Vec2 sep = separate(boids);
        Vec2 ali = align(boids);
        Vec2 coh = cohesion(boids);

        // --- apply weights ---
        sep *= sepWeight;
        ali *= aliWeight;
        coh *= cohWeight;

        // --- accumulate forces ---
        applyForce(sep);
        applyForce(ali);
        applyForce(coh);

        // --- integrate ---
        update();
        borders(w, h);

        // --- render ---
        render(gfx);
    }
};
