#include <windows.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// Настройки захвата
const int SCAN_W = 400;
const int SCAN_H = 400;

// Параметры "эффективности"
const int TARGET_COLOR_R = 255; 
const int TARGET_COLOR_G = 255;
const int TARGET_COLOR_B = 0;
const int TOLERANCE = 25;

const float FOV_RADIUS = 150.0f; // Радиус работы аима от центра экрана
const int MIN_BODY_PIXELS = 180; // Минимальный размер игрока
const float RATIO_THRESHOLD = 1.3f; // Вертикальность (Игрок выше, чем шире)

struct Target {
    int x, y, size;
    float dist;
};

void HumanMove(int dx, int dy) {
    // Имитация инерции и кривизны движения человека
    float steps = 5.0f;
    for(int i = 1; i <= steps; i++) {
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        input.mi.dx = static_cast<long>((dx / steps) + (rand() % 3 - 1));
        input.mi.dy = static_cast<long>((dy / steps) + (rand() % 3 - 1));
        SendInput(1, &input, sizeof(INPUT));
        if(i < steps) Sleep(1);
    }
}

void Action(bool shoot) {
    INPUT inputs[2] = {0};
    inputs[0].type = INPUT_MOUSE;
    inputs[1].type = INPUT_MOUSE;
    if(shoot) {
        inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &inputs[0], sizeof(INPUT));
        Sleep(20 + rand() % 30);
        // Микро-компенсация отдачи: тянем мышь вниз после выстрела
        INPUT recoil = {0};
        recoil.type = INPUT_MOUSE;
        recoil.mi.dwFlags = MOUSEEVENTF_MOVE;
        recoil.mi.dy = 2;
        SendInput(1, &recoil, sizeof(INPUT));
        SendInput(1, &inputs[1], sizeof(INPUT));
    }
}

int main() {
    srand(GetTickCount());
    std::cout << "--- CASEBOT V2: MAXIMUM EFFICIENCY ---" << std::endl;
    std::cout << "STATUS: READY. PRESS 'Q' TO ABORT." << std::endl;

    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);
    HDC hdc = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, SCAN_W, SCAN_H);
    SelectObject(memDC, hBitmap);

    while (!(GetAsyncKeyState('Q') & 0x8000)) {
        BitBlt(memDC, 0, 0, SCAN_W, SCAN_H, hdc, screenX/2 - SCAN_W/2, screenY/2 - SCAN_H/2, SRCCOPY);
        BITMAPINFOHEADER bi = {sizeof(BITMAPINFOHEADER), SCAN_W, -SCAN_H, 1, 32, BI_RGB};
        std::vector<uint32_t> pixels(SCAN_W * SCAN_H);
        GetDIBits(memDC, hBitmap, 0, SCAN_H, pixels.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        int minX = SCAN_W, maxX = 0, minY = SCAN_H, maxY = 0, count = 0;
        for (int y = 0; y < SCAN_H; y++) {
            for (int x = 0; x < SCAN_W; x++) {
                uint32_t p = pixels[y * SCAN_W + x];
                int r = (p >> 16) & 255, g = (p >> 8) & 255, b = p & 255;
                if (abs(r - TARGET_COLOR_R) < TOLERANCE && abs(g - TARGET_COLOR_G) < TOLERANCE && abs(b - TARGET_COLOR_B) < TOLERANCE) {
                    float distFromCenter = sqrt(pow(x - SCAN_W/2, 2) + pow(y - SCAN_H/2, 2));
                    if(distFromCenter < FOV_RADIUS) {
                        minX = std::min(minX, x); maxX = std::max(maxX, x);
                        minY = std::min(minY, y); maxY = std::max(maxY, y);
                        count++;
                    }
                }
            }
        }

        if (count > MIN_BODY_PIXELS) {
            int w = maxX - minX, h = maxY - minY;
            if (h > w * RATIO_THRESHOLD) {
                int targetX = (minX + maxX) / 2 - SCAN_W / 2;
                int targetY = minY + (h / 6) - SCAN_H / 2; // Целимся четко в верхнюю часть (голова)

                HumanMove(targetX, targetY);
                if (abs(targetX) < 15 && abs(targetY) < 15) {
                    Action(true);
                    Sleep(150 + rand() % 100);
                }
            }
        } else {
            // Поиск врага: медленный поворот
            static int spin = 3;
            INPUT rotate = {0};
            rotate.type = INPUT_MOUSE;
            rotate.mi.dwFlags = MOUSEEVENTF_MOVE;
            rotate.mi.dx = spin;
            SendInput(1, &rotate, sizeof(INPUT));
        }
        Sleep(1);
    }

    ReleaseDC(NULL, hdc);
    DeleteDC(memDC);
    DeleteObject(hBitmap);
    return 0;
}
