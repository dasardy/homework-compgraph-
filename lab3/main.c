#include <windows.h>
#include <gl/gl.h>
#include "stb-master/stb_easy_font.h"
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
int WindowHeight = 700;
int WindowWeight = 1000;
int isPlayed = 0;
int flag = 0;
//-------------------------ALL STRUCTURES
//button:
typedef struct{
    char name[20];
    float vert[8];
    BOOL hover;
    BOOL visible;
}
TButton;
//texture:
typedef struct {
    unsigned int texture;
} Texture;
//main character
typedef struct
{
    int isFalled;
    float currentFrame;
    int currentTypeAnimation;
    float x, y, dx, dy; //обе координаты и скорость изменения
} Hero;
//------------------------ALL ABOUT BUTTON
void TButton_Add(TButton *btn, char *name, int x, int y, int width, int height, float scale);
TButton btn[50];
int btnCnt = sizeof(btn)/sizeof(btn[0]);
BOOL PointInButton(int x, int y, TButton btn)
{
    return (x > btn.vert[0]) && (x < btn.vert[4]) &&
           (y > btn.vert[1]) && (y < btn.vert[5]);
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
         num_quads = stb_easy_font_print(0, 0, name, 0, buffer, sizeof(buffer)); // запись координат вершин элементов имени
         float textPosX = x +(width-stb_easy_font_width(name)*scale)/2.0;
         float textPosY = y +(height-stb_easy_font_height(name)*scale)/2.0;
         textPosY+= scale*2;
         glEnableClientState(GL_VERTEX_ARRAY);
            if (btn->hover)
                glColor3f(1,1,1);
            else glColor3f(0.36,0.48,0.35);
            glVertexPointer(2,GL_FLOAT,0,btn->vert);
            glDrawArrays(GL_TRIANGLE_FAN,0,4);
            glColor3f(1,1,1); //цвет обводки
            glLineWidth(4); // толщина обводки кнопки
            glDrawArrays(GL_LINE_LOOP,0,4); //отрисовка обводки
         glDisableClientState(GL_VERTEX_ARRAY);
         glPushMatrix(); //матрицу в стек
         glColor3f(0.71, 0.32, 0.94); //цвет текста
         glTranslatef(textPosX,textPosY,0); //перенос матрицы для отрисовки текста
         glScalef(scale,scale,1); //масштабирование текста
         glEnableClientState(GL_VERTEX_ARRAY); // разрешение
         glVertexPointer(2, GL_FLOAT, 16, buffer); //вектор для отрисовки
         glDrawArrays(GL_QUADS, 0, num_quads*4); //отрисовка текста
         glDisableClientState(GL_VERTEX_ARRAY);
         glPopMatrix();
         glColor3f(1.0,1.0,1.0);
     }
};
//------------------------ALL ABOT TEXTURES
Texture tex_Character;
Texture tex_Background;
void createTexture(Texture *obj, const char *path)
{
    int h_image, w_image, cnt;
    unsigned char *data = stbi_load(path, &w_image, &h_image, &cnt, 0);

    glGenTextures(1, &obj->texture);
    glBindTexture(GL_TEXTURE_2D, obj->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_image, h_image,
            0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void RenderAnimatedTexture(Texture *obj, int x, int y, int n, int t, BOOL visible)
{
    if (visible)
    {
        static float vertex[8]; //вектор текстурируемого многоугольника
        static float TexCord[]= {0,0, 1,0, 1,1, 0,1}; // текстурные координаты изображения

        static float spriteXsize=800; //переменные с размерами текстуры
        static float spriteYsize=450; //и отдельного кадра
        static float charsizey=150;
        static float charsizex=100;

        vertex[0]=vertex[6]=x;
        vertex[2]=vertex[4]=x + charsizex;
        vertex[1]=vertex[3]=y;
        vertex[5]=vertex[7]=y + charsizey;

        float vrtcoord_left = (charsizex*n)/spriteXsize; //вычисление координат кадра на изображении от
        float vrtcoord_right = vrtcoord_left+(charsizex/spriteXsize); //номера кадра
        float vrtcoord_top = (charsizey*t)/spriteYsize;
        float vrtcoord_bottom = vrtcoord_top+(charsizey/spriteYsize);

        if (GetKeyState(VK_LEFT)<0||flag==1)
        {
             TexCord[1] = TexCord[3] = vrtcoord_top; // запись в вектор текстурных координат
             TexCord[5] = TexCord[7] = vrtcoord_bottom;
             TexCord[0] = TexCord[6] = vrtcoord_right;
             TexCord[2] = TexCord[4] = vrtcoord_left;
        }
        else
        {
            TexCord[1] = TexCord[3] = vrtcoord_top; // запись в вектор текстурных координат
            TexCord[5] = TexCord[7] = vrtcoord_bottom;
            TexCord[2] = TexCord[4] = vrtcoord_right;
            TexCord[0] = TexCord[6] = vrtcoord_left;
        }




        glEnable(GL_TEXTURE_2D); //разрешение использования текстуры
        glBindTexture(GL_TEXTURE_2D, obj->texture);
        glEnable(GL_ALPHA_TEST); // проверка на элементы α-канала (не обязательно)
        glAlphaFunc(GL_GREATER, 0.99); // задается типе уровня и его числовая граница

        glPushMatrix();
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            glVertexPointer(2,GL_FLOAT,0,vertex);
            glTexCoordPointer(2, GL_FLOAT, 0, TexCord);
            glDrawArrays(GL_TRIANGLE_FAN,0,4);

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_ALPHA_TEST);
        glPopMatrix();
    }
}

void RenderStaticTexture(Texture *obj, int x, int y, int weight, int height, BOOL visible)
{
    if (visible)
    {
        static float vertex[8]; //вектор текстурируемого многоугольника
        static float TexCord[]= {0,0, 1,0, 1,1, 0,1}; // текстурные координаты изображения

        vertex[0]=vertex[6]=x;
        vertex[2]=vertex[4]=x + weight;
        vertex[1]=vertex[3]=y;
        vertex[5]=vertex[7]=y + height;

        glEnable(GL_TEXTURE_2D); //разрешение использования текстуры
        glBindTexture(GL_TEXTURE_2D, obj->texture);
        glEnable(GL_ALPHA_TEST); // проверка на элементы α-канала (не обязательно)
        glAlphaFunc(GL_GREATER, 0.99); // задается типе уровня и его числовая граница

        glPushMatrix();
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            glVertexPointer(2,GL_FLOAT,0,vertex);
            glTexCoordPointer(2, GL_FLOAT, 0, TexCord);
            glDrawArrays(GL_TRIANGLE_FAN,0,4);

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_ALPHA_TEST);
        glPopMatrix();
    }
}
//------------------------CHARACTER
Hero character;
float speed = 11; //скорость изменения координат
float gravity = -1; // скорость изменения координаты Y

void Hero_Move(Hero *obj)
{
    if (obj->y < WindowHeight - 200)
    obj->y+=obj->dy; //обновление координат
    obj->dy-=gravity; // влияние примера гравитации
    obj->currentFrame += 0.2;
    obj->currentTypeAnimation = 2;
    if (obj->currentFrame > 8)
            obj->currentFrame = 7;
    if (GetKeyState(VK_LEFT)<0 && (obj->x > 0) && isPlayed==1) // проверка состояния
    {
        if (obj->currentFrame==7)
        {
            obj->currentFrame = 1;
        }
        flag =1;
        obj->currentFrame += 0.2; // подсчет для смены кадра по спрайт
        obj->dx-=speed; // пересылка изменения координаты
        obj->x+=obj->dx; //вычисление конечной координаты
        obj->dx=0; // обнуление для исключение пересчета
        obj->currentTypeAnimation = 0;
        if (obj->currentFrame > 8) obj->currentFrame = 1; //обнуление номера кадра для
    }
    if (GetKeyState(VK_RIGHT)<0 && (obj->x < WindowWeight-113) && isPlayed==1)
    {
        if (obj->currentFrame==7)
        {
            obj->currentFrame = 1;
        }
        flag = 0;
        obj->currentFrame += 0.2;
        obj->dx+=speed;
        obj->x+=obj->dx;
        obj->dx=0;
        obj->currentTypeAnimation = 0;
        if (obj->currentFrame > 8)
            obj->currentFrame = 1;
    }
    if (GetKeyState(VK_UP)<0 && obj->y>400 && isPlayed==1)
    {
        if (obj->currentFrame==7)
        {
            obj->currentFrame = 1;
        }
        obj->currentFrame += 0.2;
        obj->dy =speed * -1; // дает импульс для имитации прыжка
        obj->y += obj->dy; //расчет координаты Y
        obj->currentTypeAnimation = 1;
        if (obj->currentFrame > 8) obj->currentFrame = 1;
    }
}

void characterUpdate(Hero *obj)
{
    Hero_Move(&character);
    RenderAnimatedTexture(&tex_Character, obj->x,obj->y, character.currentFrame, character.currentTypeAnimation,TRUE);
}
//------------------------MAINWINDOW

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
BOOL showTexture = FALSE;

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
                          "Lab num3",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          WindowWeight,//Размер окна
                          WindowHeight,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
    ShowWindow(hwnd, nCmdShow);
    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    createTexture(&tex_Character, "images/sprite.png");
    createTexture(&tex_Background, "images/background.jpg");
    RECT rct; //создание переменной с координатами прямоуголника
    GetClientRect(hwnd, &rct); //получение текущих координат окна
    glOrtho(0, rct.right, 0, rct.bottom, 1, -1); //выставляем их как координаты окна
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
            glOrtho(0, rct.right, rct.bottom, 0, 1, -1); //выставляем их как координаты окна
            TButton_Add(&btn[1], "EXIT", 425, 400, 150, 50, 2);
            TButton_Add(&btn[2], "SETTINGS", 425, 300, 150, 50, 2);
            TButton_Add(&btn[3], "PLAY", 425, 200, 150, 50, 2);
            if (isPlayed ==0)
            {
                RenderStaticTexture(&tex_Background, 0,0, WindowWeight, WindowHeight, FALSE);
                btn[1].visible = TRUE;
                btn[2].visible = TRUE;
                btn[3].visible = TRUE;
                btn[4].visible = FALSE;
            }
            if (isPlayed==1)
            {
                RenderStaticTexture(&tex_Background, 0,0, WindowWeight, WindowHeight, TRUE);
                characterUpdate(&character);
                btn[3].visible=FALSE;
                btn[2].visible=FALSE;
                TButton_Add(&btn[4], "BACK", 425, 10, 150, 50, 2);
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
            Sleep (70);
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





