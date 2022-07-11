#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};


bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
};


AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    //update running time
    data.runningTime += deltaTime;
        if(data.runningTime >= data.updateTime)
        {
            data.runningTime = 0;
            // update animatiom frame
            data.rec.x = data.frame * data.rec.width;
            data.frame++;
            if( data.frame > maxFrame)
            {
                data.frame = 0;
            }
        }
        return data;
};

int main()
{
    // window creation
    int windowDimension[2];
    windowDimension[0] = 800;
    windowDimension[1] = 450;

    InitWindow( windowDimension[0], windowDimension[1], "Runner game");

    // acceleration due to gravity
    const int gravity{1'000};
    const int jumpVel{-600};
    bool isInAir{};

    // Hero Texture and data
    Texture2D scarfy = LoadTexture("textures/scarfy.png");

    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width / 6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x =  windowDimension[0] / 2 - scarfyData.rec.width / 2;
    scarfyData.pos.y = windowDimension[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0 / 12.0;
    scarfyData.runningTime = 0.0;

    // Ennemy Texture and data
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");


    const int sizeOfNebulae{3};

    // while(sizeOfNebulae<50)
    // {
    //     sizeOfNebulae++;
    // }

    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width / 8.0;
        nebulae[i].rec.height = nebula.height / 8.0;
        nebulae[i].pos.y = windowDimension[1] - nebula.height / 8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 0.0;
        nebulae[i].pos.x = windowDimension[0] + i * 300;
    }

    float finishLine{nebulae[sizeOfNebulae - 1].pos.x};

    // Ennemy velocity
    int nebulaVel{-200};

    // rectangle dimension
    int velocity{0};

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};
    Texture2D backgroundBack = LoadTexture("textures/back-buildings.png");
    float mgX{};
    Texture2D backgroundFore = LoadTexture("textures/foreground.png");
    float fgX{};

    bool collision{};

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        const float dT{GetFrameTime()};
        BeginDrawing();
        ClearBackground(WHITE);

        //Reset backgrounds
        bgX -= 20 * dT;
        if(bgX <= -background.width * 2)
        {
            bgX = 0.0;
        }
        mgX -= 40 * dT;
        if(mgX <= -backgroundBack.width * 2)
        {
            mgX = 0.0;
        }
        fgX -= 80 * dT;
        if(fgX <= -backgroundFore.width * 2)
        {
            fgX = 0.0;
        }

        //draw background
        Vector2 bg1pos{bgX, 0.0};
        DrawTextureEx(background, bg1pos, 0.0, 3.0, WHITE);
        Vector2 bg2pos{bgX + background.width * 2 , 0.0};
        DrawTextureEx(background, bg2pos, 0.0, 3.0, WHITE);

        //draw backgroundBack
        Vector2 bgb1pos{mgX, 0.0};
        DrawTextureEx(backgroundBack, bgb1pos, 0.0, 3.0, WHITE);
        Vector2 bgb2pos{mgX + backgroundBack.width * 2 , 0.0};
        DrawTextureEx(backgroundBack, bgb2pos, 0.0, 3.0, WHITE);

        //draw backgroundFore
        Vector2 bgf1pos{fgX, 0.0};
        DrawTextureEx(backgroundFore, bgf1pos, 0.0, 3.0, WHITE);
        Vector2 bgf2pos{fgX + backgroundFore.width * 2 , 0.0};
        DrawTextureEx(backgroundFore, bgf2pos, 0.0, 3.0, WHITE);

        GetFrameTime();

        // ground check
        if(isOnGround(scarfyData, windowDimension[1]))
        {
            // on the ground
            velocity = 0;
            isInAir = false;
        }
        else
        {
            //in the air
            //apply gravity
            velocity += gravity * dT;
            isInAir = true;
        }

        if(IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVel;
            scarfyData.frame = 0;
        }

        // update nebulae positions
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i].pos.x += nebulaVel * dT;
        }

        // update hero position
        scarfyData.pos.y += velocity * dT;

        //update finishline
        finishLine += nebulaVel * dT;

        // update animatiom frame
        if(!isInAir)
        {
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }

        for (int i = 0; i < sizeOfNebulae; i++)
        {

            // update ennemy frame
            if(nebulae[i].runningTime >= nebulae[i].updateTime)
            {
                nebulae[i] = updateAnimData(nebulae[i], dT, 7);
            }
        }

        for( AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
                };

            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            if(CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }

        if(collision)
        {
            DrawText("Game over !", windowDimension[0] / 4, windowDimension[1] / 2, 70, RED);
        }
        else if(scarfyData.pos.x >= finishLine + 100)
        {
            DrawText("You win!", windowDimension[0] / 4, windowDimension[1] / 2, 70, GREEN);
        }
        else
        {
            // ennemy draw
            for (int i = 0; i < sizeOfNebulae; i++)
            {
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
            // hero draw
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);


        }



        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(backgroundBack);
    UnloadTexture(backgroundFore);
    CloseWindow();
}