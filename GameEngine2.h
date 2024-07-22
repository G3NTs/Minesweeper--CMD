#pragma once
#pragma comment(lib, "winmm.lib")

#ifndef UNICODE
#error Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode. Thanks! - Javidx9
#endif

#include <windows.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <string>


class GameEngine
{
protected:
    int nFieldWidth;
    int nFieldHeight;
    int nScreenWidth;
    int nScreenHeight;
    int mousePosX;
    int mousePosY;


    int timeEnd;

    wchar_t* screen;
    vector<int> pField;

    struct sKeyState
    {
        bool bPressed;
        bool bReleased;
        bool bHeld;
    } keys[256], mouse[5];

    bool mouseOldState[5] = { 0 };
    bool mouseNewState[5] = { 0 };
    bool bConsoleInFocus = true;
    bool GameActive = false;

    HANDLE hConsole;
    HANDLE hConsoleIn;
    SMALL_RECT rectWindow;
    COORD coord;
    DWORD dwBytesWritten;

public:
    GameEngine() // constructor
    {
        nFieldWidth = 10;
        nFieldHeight = 10;
        nScreenWidth = 12;
        nScreenHeight = 12;

        mousePosX = 0;
        mousePosY = 0;
    }

    int ConstructConsole(int m_nFieldWidth, int m_nFieldHeight)
    {
        nFieldWidth = m_nFieldWidth;
        nFieldHeight = m_nFieldHeight;
        nScreenWidth = m_nFieldWidth + 2;
        nScreenHeight = m_nFieldHeight + 4;

        // allocating all zeros to pointer matrix pField to match the screen size.
        // allocating empty spaces to all indexes of the screen.

        pField.resize(nScreenWidth * nScreenHeight, 0);
        screen = new wchar_t[nScreenWidth * nScreenHeight];
        for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
        {
            screen[i] = L' ';
        }

        // Change console visual size to a minimum so ScreenBuffer can shrink
        // below the actual visual size
        rectWindow = { 0, 0, 1, 1 };
        coord = { (short)nScreenWidth, (short)nScreenHeight };

        EngineHandleSetup();
        return 1;
    }

    void EngineHandleSetup()
    {
        // ===== SCREEN_BUFFER & HANDLES ===== //

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

        SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);

        // Set screen buffer size
        if (!SetConsoleScreenBufferSize(hConsole, coord))
        {
            cerr << "Error in: SetConsoleScreenBufferSize" << endl;
            exit(-1);
        }

        // Assign screen buffer to the console
        if (!SetConsoleActiveScreenBuffer(hConsole))
        {
            cerr << "Error in: SetConsoleActiveScreenBuffer" << endl;
            exit(-1);
        }

        // Set Physical Console Window Size
        rectWindow = { 0, 0, (short)(nScreenWidth - 1), (short)(nScreenHeight - 1) };
        if (!SetConsoleWindowInfo(hConsole, TRUE, &rectWindow))
        {
            cerr << "Error in: SetConsoleWindowInfo" << endl;
            exit(-1);
        }

        // Set flags to allow mouse input		
        if (!SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
        {
            cerr << "Error in: SetConsoleMode" << endl;
            exit(-1);
        }

    }

    void InputEventManager()
    {
        INPUT_RECORD inBuf[32];
        DWORD events = 0;
        GetNumberOfConsoleInputEvents(hConsoleIn, &events);

        if (events > 0)
        {
            ReadConsoleInput(hConsoleIn, inBuf, events, &events);
        }

        // Handle events - we only care about mouse clicks and movement
        for (DWORD i = 0; i < events; i++)
        {
            switch (inBuf[i].EventType)
            {
            case FOCUS_EVENT:
            {
                bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
            }
            break;

            case MOUSE_EVENT:
            {
                switch (inBuf[i].Event.MouseEvent.dwEventFlags)
                {
                case MOUSE_MOVED:
                {
                    mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
                    mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
                }
                break;

                case 0:
                {
                    for (int m = 0; m < 5; m++)
                    {
                        mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
                    }
                }
                break;

                default:
                    break;
                }
            }
            break;

            default:
                break;
                // We don't care just at the moment
            }
        }

        // Check mouse buttons 0 to 4
        for (int m = 0; m < 5; m++)
        {
            mouse[m].bPressed = false;
            mouse[m].bReleased = false;

            if (mouseNewState[m] != mouseOldState[m])
            {
                if (mouseNewState[m])
                {
                    mouse[m].bPressed = true;
                    mouse[m].bHeld = true;
                }
                else
                {
                    mouse[m].bReleased = true;
                    mouse[m].bHeld = false;
                }
            }

            mouseOldState[m] = mouseNewState[m];
        }
    }

    void UpdateDisplay()
    {
        // ===== DISPLAY ===== //

        for (int x = 0; x < nFieldWidth + 2; x++)
        {
            for (int y = 0; y < nFieldHeight + 4; y++)
            {
                screen[y * nScreenWidth + x] = L" 0123456789#?*FYou Win@LoseB:T¤-"[pField[y * (nFieldWidth + 2) + x]];
            }
        }

        // Update Display Frame
        {
            WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, {0,0}, &dwBytesWritten);
        }
    }

    void Start()
    {
        // Start the thread
        GameActive = true;
        thread t = thread(&GameEngine::GameThread, this);

        // Wait for thread to be exited
        t.join();
    }

private:
    void GameThread()
    {
        // Create user resources as part of this thread
        if (!OnUserCreate())
        {
            GameActive = false;
        }

        auto tp1 = std::chrono::system_clock::now();
        auto tp2 = std::chrono::system_clock::now();

        while (GameActive)
        {
            // Handle Timing
            tp2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsedTime = tp2 - tp1;
            tp1 = tp2;
            float fElapsedTime = elapsedTime.count();

            InputEventManager();

            // Handle Frame Update
            if (!OnUserUpdate(fElapsedTime))
            {
                GameActive = false;
            }
                
            UpdateDisplay();
        }

    }
public:
    // User MUST OVERRIDE THESE!!
    virtual bool OnUserCreate() = 0;
    virtual bool OnUserUpdate(float fElapsedTime) = 0;
};