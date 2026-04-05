#include "raylib.h"

int main(void)
{
    // 1. إعدادات النافذة
    const int screenWidth = 1280;
    const int screenHeight = 720;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "TP Physics - Interactive Photogates");

    // 2. إعداد الكاميرا
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 15.0f, 15.0f, 15.0f };
    camera.target = (Vector3){ 0.0f, 3.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // 3. تحميل الموديل
    Model myModel = LoadModel("resources/tp.glb");

    // 4. متغيرات الفيزياء
    Vector3 initialPos = { 0.0f, 10.0f, -0.8f };
    Vector3 ballPosition = initialPos;
    float velocityY = 0.0f;
    float gravity = -0.0027f;
    float tableHeight = 5.2f;
    bool isFalling = false;

    // 5. متغيرات الوقت والبوابات (قابلة للتعديل)
    float timer = 0.0f;
    bool timerStarted = false;
    bool timerFinished = false;

    float gate1Height = 8.5f; // ارتفاع البوابة 1 (تتحرك بالأسهم)
    float gate2Height = 6.5f; // ارتفاع البوابة 2 (تتحرك بـ W/S)

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // أ- تحديث الكاميرا (زر الماوس الأيمن)
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))  UpdateCamera(&camera, CAMERA_ORBITAL); 

        // ب- تحريك البوابات (قبل البدء بالسقوط)
        if (!isFalling) {
            // تحريك البوابة العليا
            if (IsKeyDown(KEY_UP)) gate1Height += 0.05f;
            if (IsKeyDown(KEY_DOWN)) gate1Height -= 0.05f;
             
            // تحريك البوابة السفلى
            if (IsKeyDown(KEY_W)) gate2Height += 0.05f;
            if (IsKeyDown(KEY_S)) gate2Height -= 0.05f;
        }

        // ج- التحكم بالسقوط والإعادة
        if (IsKeyPressed(KEY_SPACE)) isFalling = true;

        if (IsKeyPressed(KEY_R)) {
            isFalling = false;
            velocityY = 0.0f;
            ballPosition = initialPos;
            timer = 0.0f;
            timerStarted = false;
            timerFinished = false;
        }

        // د- منطق الفيزياء والوقت الديناميكي
        if (isFalling) {
            velocityY += gravity;
            ballPosition.y += velocityY;

            // نظام الوقت المعتمد على المواقع الحالية للبوابات
            if (ballPosition.y <= gate1Height && !timerStarted && !timerFinished) {
                timerStarted = true;
            }

            if (timerStarted && !timerFinished) {
                timer += GetFrameTime();
            }

            if (ballPosition.y <= gate2Height && timerStarted) {
                timerFinished = true;
                timerStarted = false;
            }

            // التصادم مع الطاولة
            if (ballPosition.y <= tableHeight) {
                ballPosition.y = tableHeight;
                velocityY *= -0.4f;
                if (velocityY > -0.01f && velocityY < 0.01f) {
                    velocityY = 0;
                    isFalling = false;
                }
            }
        }

        // هـ- الرسم
        BeginDrawing();
        ClearBackground(DARKGRAY);

        BeginMode3D(camera);
        DrawModel(myModel, (Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);

        // رسم الكرة
        DrawSphere(ballPosition, 0.15f, YELLOW);

        // رسم "مؤشرات" البوابات (تتحرك مع المتغيرات)
        // البوابة 1
        DrawCubeWires((Vector3) { 0.0f, gate1Height, -0.8f }, 1.2f, 0.1f, 1.2f, SKYBLUE);
        // البوابة 2
        DrawCubeWires((Vector3) { 0.0f, gate2Height, -0.8f }, 1.2f, 0.1f, 1.2f, BLUE);

        DrawGrid(10, 1.0f);
        EndMode3D();

        // واجهة المستخدم (UI)
        DrawText("Controls:", 10, 10, 20, RAYWHITE);
        DrawText("- SPACE: Start Fall", 10, 35, 18, LIGHTGRAY);
        DrawText("- R: Reset Simulation", 10, 55, 18, LIGHTGRAY);
        DrawText("- ARROWS: Move Gate 1", 10, 75, 18, SKYBLUE);
        DrawText("- W/S: Move Gate 2", 10, 95, 18, BLUE);

        // عرض الوقت والارتفاعات
        int seconds = (int)timer;
        int ms = (int)((timer - seconds) * 1000);
        DrawText(TextFormat("TIME: %02d:%03d s", seconds, ms), 10, 130, 40, YELLOW);

        DrawText(TextFormat("Gate 1: %.2f m", gate1Height), screenWidth - 200, 20, 20, SKYBLUE);
        DrawText(TextFormat("Gate 2: %.2f m", gate2Height), screenWidth - 200, 50, 20, BLUE);
        DrawText(TextFormat("Distance: %.2f m", gate1Height - gate2Height), screenWidth - 200, 80, 20, GREEN);

        if (timerFinished) DrawText("RESULT SAVED", 10, 180, 25, GREEN);

        EndDrawing();
    }

    UnloadModel(myModel);
    CloseWindow();
    return 0;
}