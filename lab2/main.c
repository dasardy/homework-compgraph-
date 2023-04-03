#include <windows.h>
#include <gl/gl.h>
#include "stb-master/stb_easy_font.h"
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
typedef struct{
    char name[20];
    float vert[8];
    BOOL hover;
    BOOL visible;
}
TButton;
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
void TButton_Add(TButton *btn, char *name, int x, int y, int width, int height, float scale);
TButton btn[50];
int btnCnt = sizeof(btn)/sizeof(btn[0]);

BOOL PointInButton(int x, int y, TButton btn)
{
    return (x > btn.vert[0]) && (x < btn.vert[4]) &&
           (y > btn.vert[1]) && (y < btn.vert[5]);
}
BOOL showTexture = FALSE;
int isPlayed = 0;
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;
    if (!RegisterClassEx(&wcex))
        return 0;
    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "Lab num2",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1000,//������ ����
                          700,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
    ShowWindow(hwnd, nCmdShow);
    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    RECT rct; //�������� ���������� � ������������ �������������
    GetClientRect(hwnd, &rct); //��������� ������� ��������� ����
    glOrtho(0, rct.right, 0, rct.bottom, 1, -1); //���������� �� ��� ���������� ����
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */
            int a = 1;
            glClearColor(0.53,0.64,0.71,1);
            glClear(GL_COLOR_BUFFER_BIT);
            glPushMatrix();
            glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
            glLoadIdentity();
            glOrtho(0, rct.right, rct.bottom, 0, 1, -1); //���������� �� ��� ���������� ����
            TButton_Add(&btn[1], "EXIT", 425, 400, 150, 50, 2);
            TButton_Add(&btn[2], "SETTINGS", 425, 300, 150, 50, 2);
            TButton_Add(&btn[3], "PLAY", 425, 200, 150, 50, 2);
            if (isPlayed ==0)
            {
                btn[1].visible = TRUE;
                btn[2].visible = TRUE;
                btn[3].visible = TRUE;
                btn[4].visible = FALSE;
            }

            if (isPlayed==1)
            {
                btn[3].visible=FALSE;
                btn[2].visible=FALSE;
                TButton_Add(&btn[4], "BACK", 425, 300, 150, 50, 2);
                btn[4].visible=TRUE;
            }
            if (isPlayed==2)
            {
                btn[1].visible=TRUE;
                btn[2].visible=FALSE;
                btn[3].visible=FALSE;
                TButton_Add(&btn[4], "BACK", 425, 300, 150, 50, 2);
                btn[4].visible=TRUE;
            }
            glPopMatrix();
            SwapBuffers(hDC);
            Sleep (100);
        }
    }
    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);
    /* destroy the window explicitly */
    DestroyWindow(hwnd);
    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_LBUTTONDOWN:
            for (int i = 1; i < btnCnt; i++)
                if (PointInButton(LOWORD(lParam), HIWORD(lParam), btn[i]))
                {
                    if ((strcmp(btn[i].name, "EXIT") == 0) && (btn[i].visible ==TRUE))
                    {
                        printf("EXIT\n");
                        PostQuitMessage(0);

                    }
                    if ((strcmp(btn[i].name, "BACK") == 0) && (btn[i].visible ==TRUE))
                    {
                        isPlayed = 0;
                        printf("BACK\n");
                    }

                    else if ((strcmp(btn[i].name, "SETTINGS") == 0)&& (btn[i].visible ==TRUE))
                    {
                        printf("OPEN SETTINGS\n");
                        isPlayed = 2;
                    }
                    else if ((strcmp(btn[i].name, "PLAY") == 0) && (btn[i].visible ==TRUE))
                    {
                        printf("YEAH, LET'S GO!\n");
                        isPlayed = 1;
                    }
                }
        break;
        case WM_MOUSEMOVE:
            for (int i = 1; i < btnCnt; i++)
                btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), btn[i]);
        break;
        case WM_DESTROY:
            return 0;
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;
                case WM_MOUSEHOVER:
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;
    /* get the device context (DC) */
    *hDC = GetDC(hwnd);
    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, iFormat, &pfd);
    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}
void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}


void TButton_Add(TButton *btn, char *name, int x, int y, int width, int height, float scale)
{
    strcpy(btn->name, name);
    if (btn->visible){
        float buffer[1000];
        int num_quads;
            btn->vert[0]=btn->vert[6]=x;
            btn->vert[2]=btn->vert[4]=x + width;
            btn->vert[1]=btn->vert[3]=y;
            btn->vert[5]=btn->vert[7]=y + height;
         num_quads = stb_easy_font_print(0, 0, name, 0, buffer, sizeof(buffer)); // ������ ��������� ������ ��������� �����
         float textPosX = x +(width-stb_easy_font_width(name)*scale)/2.0;
         float textPosY = y +(height-stb_easy_font_height(name)*scale)/2.0;
         textPosY+= scale*2;
         glEnableClientState(GL_VERTEX_ARRAY);
            if (btn->hover)
                glColor3f(1,1,1);
            else glColor3f(0.36,0.48,0.35);
            glVertexPointer(2,GL_FLOAT,0,btn->vert);
            glDrawArrays(GL_TRIANGLE_FAN,0,4);
            glColor3f(1,1,1); //���� �������
            glLineWidth(4); // ������� ������� ������
            glDrawArrays(GL_LINE_LOOP,0,4); //��������� �������
         glDisableClientState(GL_VERTEX_ARRAY);
         glPushMatrix(); //������� � ����
         glColor3f(0.71, 0.32, 0.94); //���� ������
         glTranslatef(textPosX,textPosY,0); //������� ������� ��� ��������� ������
         glScalef(scale,scale,1); //��������������� ������
         glEnableClientState(GL_VERTEX_ARRAY); // ����������
         glVertexPointer(2, GL_FLOAT, 16, buffer); //������ ��� ���������
         glDrawArrays(GL_QUADS, 0, num_quads*4); //��������� ������
         glDisableClientState(GL_VERTEX_ARRAY);
         glPopMatrix();
         glColor3f(1.0,1.0,1.0);
     }
};


