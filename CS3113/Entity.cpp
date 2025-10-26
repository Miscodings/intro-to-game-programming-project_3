#include "Entity.h"

static void Normalise(Vector2 *v)
{
    float len = sqrtf(v->x * v->x + v->y * v->y); // pythag theorem
    if (len > 0.0001f)
    {
        v->x /= len;
        v->y /= len;
    }
}

Entity::Entity()
    : mPosition({0,0}), mVelocity({0,0}), mAcceleration({0,0}),
      mScale({1,1}), mColliderDimensions({50,50}), mAngle(0.0f)
{}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath)
    : mPosition(position), mVelocity({0,0}), mAcceleration({0,0}),
      mScale(scale), mAngle(0.0f)
    {
        setTexture(textureFilepath);
        mColliderDimensions = { (float)mTexture.width * scale.x, (float)mTexture.height * scale.y };
    }

Entity::~Entity()
{
    UnloadTexture(mTexture);
}

void Entity::setTexture(const char* filepath)
{
    mTexture = LoadTexture(filepath);
}

void Entity::setScale(Vector2 scale)
{
    mScale = scale;
    mColliderDimensions = { mTexture.width * scale.x, mTexture.height * scale.y };
}

void Entity::setColliderDimensions(Vector2 dim)
{
    mColliderDimensions = dim;
}

void Entity::update(float deltaTime)
{
    if (mRotateLeftFlag)  mAngle -= ROTATION_SPEED_DEG * deltaTime;
    if (mRotateRightFlag) mAngle += ROTATION_SPEED_DEG * deltaTime;
    if (mAngle > 360.0f) mAngle -= 360.0f;
    if (mAngle < 0.0f)   mAngle += 360.0f;

    mAcceleration = {0, GRAVITY};

    if (mThrusterFlag)
    {
        float rad = mAngle * (PI / 180.0f);
        Vector2 thrustDir = { sinf(rad), -cosf(rad) };
        Normalise(&thrustDir);
        mAcceleration.x += thrustDir.x * THRUST_FORCE;
        mAcceleration.y += thrustDir.y * THRUST_FORCE;
    }

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    mPosition.x += mVelocity.x * deltaTime;
    mPosition.y += mVelocity.y * deltaTime;
}

void Entity::render()
{
    Rectangle src = {0, 0, (float)mTexture.width, (float)mTexture.height};
    Rectangle dest = {mPosition.x, mPosition.y, mColliderDimensions.x, mColliderDimensions.y};
    Vector2 origin = {mColliderDimensions.x / 2, mColliderDimensions.y / 2};

    DrawTexturePro(mTexture, src, dest, origin, mAngle, WHITE);
}
