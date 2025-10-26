#ifndef ENTITY_H
#define ENTITY_H

#include <cmath>
#include "raylib.h"

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mVelocity;
    Vector2 mAcceleration;
    Vector2 mScale;
    Vector2 mColliderDimensions;

    Texture2D mTexture;
    float mAngle;

    bool mRotateLeftFlag  = false;
    bool mRotateRightFlag = false;
    bool mThrusterFlag    = false;

    static constexpr float ROTATION_SPEED_DEG = 120.0f;
    static constexpr float THRUST_FORCE       = 900.0f;
    static constexpr float GRAVITY            = 100.0f;

public:
    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath);
    ~Entity();

    void update(float deltaTime);
    void render();

    void rotateLeft()    { mRotateLeftFlag  = true; }
    void rotateRight()   { mRotateRightFlag = true; }
    void engageThruster(){ mThrusterFlag    = true; }
    void clearInputFlags(){ mRotateLeftFlag = mRotateRightFlag = mThrusterFlag = false; }

    Vector2 getPosition() const { return mPosition; }
    float   getAngle()    const { return mAngle;    }

    void setPosition(Vector2 pos) { mPosition = pos; }
    void setAngle(float angle)    { mAngle = angle;  }
    void setTexture(const char* filepath);
    void setScale(Vector2 scale);
    void setColliderDimensions(Vector2 dim);
};

#endif
