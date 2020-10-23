/*******************
Program By CHISF
Date 2020.10.23
*******************/
#include "stdafx.h"
#include <graphics.h>
#include <Windows.h>
#include <time.h>
#include <mmsystem.h>
#include <imapi.h>
#include <conio.h>
#include <iostream>
#include <sstream>
#pragma comment(lib,"winmm.lib")
#define MATRIX_WIDTH 4
#define MATRIX_HEIGHT 4
#define BG_WIDTH 10
#define BG_HEIGHT 20
#define RECT_WIDTH 30
bool IsKeyDown(int key)
{
    return (GetAsyncKeyState(key) & 0x8000 ? 1 : 0);
}
struct MatrixData
{
    int data[MATRIX_HEIGHT][MATRIX_WIDTH];
};

struct Block
{
    int dirctionCount;  // 方向数量
    
    MatrixData matrixData[4]; // 最多4个方向数据
};

Block arrBlock[5];
int curBlockIndex = -1;
int posX = -4;
int posY = -4;
int interval = 700;
int curDownInterval = interval;
int lastMoveTick = 0;
int lastDownTick = 0;
int curTick = 0;
int curDirection = 0;
int color = GREEN;

IMAGE img;

int backGround[BG_HEIGHT][BG_WIDTH] = { 0 };

void InitBlocks()
{
    loadimage(&img, "1.jpg");

    arrBlock[0].matrixData[0] =
    {
        0, 0, 0, 0,
        0, 0, 0, 0,
        1, 1, 0, 0,
        0, 1, 1, 0,

    };

    arrBlock[0].matrixData[1] =
    {
        0, 0, 0, 0,
        0, 1, 0, 0,
        1, 1, 0, 0,
        1, 0, 0, 0,

    };
    arrBlock[0].dirctionCount = 2;

    arrBlock[1].matrixData[0] =
    {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        1, 1, 1, 1,

    };

    arrBlock[1].matrixData[1] =
    {
        0, 1, 0, 0,
        0, 1, 0, 0,
        0, 1, 0, 0,
        0, 1, 0, 0,

    };
    arrBlock[1].dirctionCount = 2;

    arrBlock[2].matrixData[0] =
    {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 1, 1, 0,
        1, 1, 0, 0,

    };

    arrBlock[2].matrixData[1] =
    {
        0, 0, 0, 0,
        1, 0, 0, 0,
        1, 1, 0, 0,
        0, 1, 0, 0,

    };
    arrBlock[2].dirctionCount = 2;

    arrBlock[3].matrixData[0] =
    {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 1, 1, 0,
        0, 1, 1, 0,

    };
    arrBlock[3].dirctionCount = 1;

    arrBlock[4].matrixData[0] =
    {
        0, 0, 0, 0,
        1, 1, 0, 0,
        0, 1, 0, 0,
        0, 1, 0, 0,

    };

    arrBlock[4].matrixData[1] =
    {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 1, 0,
        1, 1, 1, 0,

    };

    arrBlock[4].matrixData[2] =
    {
        0, 0, 0, 0,
        1, 0, 0, 0,
        1, 0, 0, 0,
        1, 1, 0, 0,

    };

    arrBlock[4].matrixData[3] =
    {
        0, 0, 0, 0,
        0, 0, 0, 0,
        1, 1, 1, 0,
        1, 0, 0, 0,

    };
    arrBlock[4].dirctionCount = 4;
}

void DrawBlock(Block &block, int posX, int posY)
{
    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
        for (int j = 0; j < MATRIX_HEIGHT; j++)
        {
            MatrixData &curDirData = block.matrixData[curDirection];
            if (curDirData.data[j][i] == 0)
            {
                continue;
            }

            int x = (i + posX) * RECT_WIDTH;
            int y = (j + posY) * RECT_WIDTH;
            setfillcolor(color);
            fillrectangle(x, y, x + RECT_WIDTH, y + RECT_WIDTH);
        }
    }
}

void DrawBackground(int backGround[BG_HEIGHT][BG_WIDTH])
{
    for (int i = 0; i < BG_WIDTH; i++)
    {
        for (int j = 0; j < BG_HEIGHT; j++)
        {
            if (backGround[j][i] != 1)
            {
                continue;
            }

            int x = i * RECT_WIDTH;
            int y = j * RECT_WIDTH;

            setlinecolor(0x555555);
            setfillcolor(0xFFFFFF);
            fillrectangle(x, y, x + RECT_WIDTH, y + RECT_WIDTH);
        }

    }

    putimage(0, 0, &img, SRCAND);
}

bool CanMove(Block &block, int x, int y)
{
    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
        for (int j = 0; j < MATRIX_HEIGHT; j++)
        {
            
            MatrixData &curDirData = block.matrixData[curDirection];
            if (curDirData.data[j][i] == 0)
            {
                continue;
            }

            if (i + x < 0)
            {
                 return false;
            }

            if (i + x >= BG_WIDTH)
            {
                return false;
            }

            if (j + y >= BG_HEIGHT)
            {
                return false;
            }

            if (backGround[j + y][i + x] == 1)
            {
                return false;
            }
        }
    }

    return true;
}

bool CheckNewTurn(Block &block)
{
    int tmpY = posY + 1;
    bool restart = false;
    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
        for (int j = 0; j < MATRIX_HEIGHT; j++)
        {
            if (j + tmpY < 0)
            {
                continue;
            }

            MatrixData &curDirData = block.matrixData[curDirection];
            if (curDirData.data[j][i] == 0)
            {
                continue;
            }

            

            if (backGround[j + tmpY][i + posX] == 1)
            {
                restart = true;
            }

            if (j + tmpY >= 20)
             {
                restart = true;
            }
        }
    }

    return restart;
}

void CreateNewBlock()
{
    curBlockIndex = rand() % _countof(arrBlock);    // 从所有方块中随机一个
    posX = 4;   // 方块初始坐标
    posY = -3;

    lastDownTick = GetTickCount();
}
LPCWSTR stringToLPCWSTR(std::string orig)
{
    size_t origsize = orig.length() + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
    mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
    return wcstring;
}
void CheckClearLine()
{
    for (int j = BG_HEIGHT - 1; j >= 0; j--)
    {
        bool isBreak = false;
        for (int i = 0; i < BG_WIDTH; i++)
        {
            if (backGround[j][i] != 1)
            {
                isBreak = true;
                break;
            }
        }

        if (!isBreak)
        {
            for (int k = j; k >= 1; --k)
            {
                for (int i = 0; i < BG_WIDTH; i++)
                {
                    backGround[k][i] = backGround[k - 1][i];
                }
                
            }

            for (int i = 0; i < BG_WIDTH; i++)
            {
                backGround[0][i] = 0;
            }
            
        }
    }
}

bool BlockDone(Block &block)
{
    bool isGameOver = false;
    for (int i = 0; i < MATRIX_WIDTH; i++)
    {
        for (int j = 0; j < MATRIX_HEIGHT; j++)
        {
            MatrixData &curDirData = block.matrixData[curDirection];
            if (curDirData.data[j][i] == 0)
            {
                continue;
            }

            if (j + posY < 0)
            {
                // 游戏结束
                isGameOver = true;
                 continue;
            }

            backGround[j + posY][i + posX] = 1;

            curDownInterval = interval;

        }
    }
    CheckClearLine();
    CreateNewBlock();
    return !isGameOver;
}



void CheckKeyboard(Block &block)
{
    if (curTick - lastMoveTick > 100)
    {
        if (IsKeyDown(VK_LEFT))
        {
            if (CanMove( block, posX - 1, posY))
            {
                posX -= 1;
                lastMoveTick = curTick;
            }
               
        }
        else if (IsKeyDown(VK_RIGHT))
        {
            if (CanMove(block, posX + 1, posY))
            {
                posX += 1;
                lastMoveTick = curTick;
            }
            
        }
        else if (IsKeyDown(VK_DOWN))
        {
            if (CanMove(block, posX, posY + 1))
            {
                posY += 1;
                lastMoveTick = curTick;
                lastDownTick = curTick;
            }
            
        }
        else if (IsKeyDown(VK_UP))
        {
            curDirection++;
            curDirection %= block.dirctionCount;

            lastMoveTick = curTick;
        }
        else if (IsKeyDown(VK_SPACE))
        {
            curDownInterval = 10;
        }
    }
}

int main()
{ 

    initgraph(RECT_WIDTH * BG_WIDTH, RECT_WIDTH * BG_HEIGHT);
    InitBlocks();
    srand(time(0));
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTitleW( L"BLOCK" );
    IMAGE buffer(RECT_WIDTH * BG_WIDTH, RECT_WIDTH * BG_HEIGHT);
    while (true)
    {
        SetWorkingImage(&buffer);
        setfillcolor(BLACK);
        fillrectangle(0, 0, RECT_WIDTH * BG_WIDTH, RECT_WIDTH * BG_HEIGHT);
        DrawBackground(backGround);
        if (curBlockIndex == -1)
        {
            CreateNewBlock();
        }
        curTick = GetTickCount();
        Block &curBlock = arrBlock[curBlockIndex];
        DrawBlock(curBlock, posX, posY);
        if (curTick - lastDownTick > curDownInterval)
        {
            bool restart = CheckNewTurn(curBlock);

            if (!restart)
            {
                posY += 1;
                lastDownTick = GetTickCount();
            }
            else
            {
                if (!BlockDone(curBlock))
                {
                    break;
                }
            }
        }       
        CheckKeyboard(curBlock);
        SetWorkingImage(NULL);
        putimage(0, 0, &buffer);

        Sleep(10);
    }
    getchar();
    closegraph();
    return 0;
}

