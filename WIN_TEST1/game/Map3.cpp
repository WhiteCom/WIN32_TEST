﻿#include "Map3.h"

#include "iWindow.h"
#include "TileType.h"
#include "Loading.h"
#include "Intro.h"

/*
   public:
   int tileX, tileY, tileWidth, tileHeight;
   int* tileIndex, *tileWeight;

   Texture** texTiles;
   int numTiles;


   int mode;// 0:tile, 1:attr, 2:obj
   int selectedTile, selectedWeight;

*/


MapEditor::MapEditor()
{
    tileX = 0;
    tileY = 0;
    tileWidth = 0;
    tileHeight = 0;
    tileIndex = NULL;
    tileWeight = NULL;

    //numTiles = 0;

    selectedTile = 0;
    selectedWeight = 0;
}

MapEditor::MapEditor(const char* szFormat, ...)
{
    tileX = 0;
    tileY = 0;
    tileWidth = 0;
    tileHeight = 0;
    tileIndex = NULL;
    tileWeight = NULL;

    //numTiles = 0;

    selectedTile = 0;
    selectedWeight = 0;

    char szText[1024];
    va_start_end(szText, szFormat);

    load(szText);
}

MapEditor::~MapEditor()
{
    clean();
}

//#issue! 현재 여기서 뭔가 잘못 삭제하고 있어서 문제가 있다. 
void MapEditor::clean()
{
    if (tileIndex == NULL)
        return;

    for (int i = 0; i < 3; i++)
        delete tileIndex[i];
    delete tileIndex;

    delete tileWeight;
}

void MapEditor::draw(float dt, iPoint off)
{
    int i, xy = tileX * tileY;
    int ti, tw;

    for (i = 0; i < xy; i++)
    {
        int x = off.x + i % tileX * tileWidth;
        int y = off.y + i / tileX * tileHeight;
        
        //if(texTiles[i]) //NULL 아닌경우만
        //    drawImage(texTiles[i], x, y, TOP | LEFT);
    }
}

void MapEditor::init(int x, int y, int w, int h)
{
    tileX = x;
    tileY = y;
    tileWidth = w;
    tileHeight = h;
    
    int tileXY = tileX * tileY;
    int i;
    tileIndex = new int* [3];
    for (i = 0; i < 3; i++)
        tileIndex[i] = new int[tileXY];

    tileWeight = new int[tileXY];
        
    //numTiles = tileXY;
}

void MapEditor::load(const char* szFormat, ...)
{
    clean();

    char szText[1024];
    va_start_end(szText, szFormat);

    FILE* pf = fopen(szText, "rb");
    if (pf == NULL)
    {
        printf("No File!");
        return;
    }

    // tile info
    fread(&tileX, sizeof(int), 1, pf);
    fread(&tileY, sizeof(int), 1, pf);
    fread(&tileWidth, sizeof(int), 1, pf);
    fread(&tileHeight, sizeof(int), 1, pf);

    int xy = tileX * tileY;
    int i;
    tileIndex = new int* [3];
    for (i = 0; i < 3; i++)
        tileIndex[i] = new int[xy];
    tileWeight = new int[xy];

    for(i = 0;i<3;i++)
        fread(tileIndex[i], sizeof(int), xy, pf);
    fread(tileWeight, sizeof(int), xy, pf);

    //fread(&numTiles, sizeof(int), xy, pf);

#if 0 //texTiles 멤버변수 안쓰면 사용안할것들
    texTiles = new Texture * [numTiles];
    int i, j;
    for (i = 0; i < numTiles; i++)
    {
        Bitmap* bmp = new Bitmap(tileWidth, tileHeight, PixelFormat32bppARGB);
        Rect rt(0, 0, tileWidth, tileHeight);
        BitmapData bd;
        bmp->LockBits(&rt, ImageLockModeWrite, PixelFormat32bppARGB, &bd);
        int stride = bd.Stride / 4;
        int* rgba = (int*)bd.Scan0;
        for (j = 0; j < tileHeight; j++)
            fread(&rgba[stride], sizeof(int), tileWidth, pf);
        bmp->UnlockBits(&bd);

        Texture* tex = new Texture;
        tex->texID = bmp;
        tex->width = tileWidth;
        tex->height = tileHeight;
        tex->potWidth = tileWidth;
        tex->potHeight = tileHeight;
        tex->retainCount = 1;

        texTiles[i] = tex;
    }
#endif

    fclose(pf);
}

void MapEditor::save(const char* str, ...)
{
    FILE* pf = fopen(str, "wb");

    // tile info
    fwrite(&tileX, sizeof(int), 1, pf);
    fwrite(&tileY, sizeof(int), 1, pf);
    fwrite(&tileWidth, sizeof(int), 1, pf);
    fwrite(&tileHeight, sizeof(int), 1, pf);

    int xy = tileX * tileY;
    for(int i=0;i<3;i++)
        fwrite(tileIndex[i], sizeof(int), xy, pf);

    fwrite(tileWeight, sizeof(int), xy, pf);

    //write(&numTiles, sizeof(int), xy, pf);
#if 0 //texTiles 멤버변수 안쓰면 사용안할것들
    int i, j;
    for (i = 0; i < numTiles; i++)
    {
        Bitmap* bmp = (Bitmap*)texTiles[i]->texID;
        Rect rt(0, 0, tileWidth, tileHeight);
        BitmapData bd;
        bmp->LockBits(&rt, ImageLockModeRead, PixelFormat32bppARGB, &bd);
        int stride = bd.Stride / 4;
        int* rgba = (int*)bd.Scan0;
        for (j = 0; j < tileHeight; j++)
            fwrite(&rgba[stride], sizeof(int), tileWidth, pf);
        bmp->UnlockBits(&bd);
    }
#endif
    fclose(pf);
}

void MapEditor::insert(iPoint point, int type)
{
    int x = point.x; x /= tileWidth;
    int y = point.y; y /= tileHeight;
    int xy = tileX * y + x;

    for (int i = 0; i < 3; i++)
    {
        if (tileIndex[i][xy] > -1) //이미 무언가 할당되어있는데, 새로 추가될때
            tileIndex[i][xy] = -1;
    }

    tileIndex[type][xy] = selectedTile;
    tileWeight[xy] = selectedWeight;
}

#if 0
char* openImg()
{
    WCHAR currPath[1024];
    GetCurrentDirectory(1024, currPath);

    OPENFILENAME ofn;
    memset(&ofn, 0x00, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    extern HWND hWnd;
    ofn.hwndOwner = hWnd;
    TCHAR szFile[260] = { 0 };
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T("All\0*.*\0Text\0*.TXT\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    char* path = NULL;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        path = utf16_to_utf8(ofn.lpstrFile);
        printf("path[%s]\n", path);

        SetCurrentDirectory(currPath);
    }

    return path;
}
#endif

//to do...
//기존 map2.cpp 에서 tEditor 내에 타일을 저장하는 방식을 바꿔야함. 
//draw에서 tex** createImage 하지 않기 -> load 등 한번만 불리는 곳에서 처리해야함
//1) 클래스 내 멤버변수로 tex** 생성 (생성자 혹은 load 등에서 처리)
//2) load 등에서 본 파일(map3.cpp) 에서만 쓰이는 파일 tex** 생성 (이미지 생성 후 객체 내부 tex** 변수에도 들어갈수 있게처리)

//==========================================================
//iRect영역들
//==========================================================

iRect E_RT;
iRect* EditRT;
iRect W_RT;
iRect* WeiRT; //가중치 선택하는 영역의 숫자칸들
iRect TileRT;
iRect TileImgRT;
iRect TileImgRT2;
iRect TileImgRT3;
iRect TileWeiRT;
iRect TileSelRT;
iRect LoadBtn;
iRect SaveBtn;
iRect ExitBtn;
iRect selectedImgRT;
iRect selectedWeiRT;
iRect ExitRT;

iPoint prevPosition;
iPoint EditRT_point;
iPoint TileRT_point;
iPoint TileImgRT_point;
iPoint TileImgRT2_point;
iPoint TileImgRT3_point;
iPoint TileWeiRT_point;
iPoint TileSelRT_point;
iPoint LoadBtn_point;
iPoint SaveBtn_point;
iPoint ExitBtn_point;
iPoint selectedImgRT_point;
iPoint selectedWeiRT_point;
iPoint ExitRT_point;

static bool set_check = false;
static bool movingTileImg = false;
static bool movingTileImg2 = false;
static bool movingTileImg3 = false;

static const char* ImgPath[3];

Texture*** texs;
Texture** tmpTileTex;
int* tmpTileWei;
Texture* selectedTex = NULL; //커서로 선택한 타일이미지
int selectedWei = 99; //INF
int texs_idx;
MapEditor* tEditor;

const char* weight[11] = {
       "0","1","2","3","4","5","6","7","8","9", "10"
};
char selWei[10] = { NULL, };
char tmpWei[10] = { NULL, };

void RTset()
{
    //좌표 세팅을 위한 일회성 함수로 한번 쓰고 안쓸거임.
    prevPosition =          iPointMake(0, 0);
    EditRT_point =          iPointMake(0, 0);
    TileRT_point =          iPointMake(tileWSize * 17,  0);
    //TileRT_point =          iPointMake(0, 0);

    TileImgRT_point =       iPointMake(0,               tileHSize * 13);
    TileImgRT2_point =      iPointMake(tileWSize * 9,   tileHSize * 13);
    TileImgRT3_point =      iPointMake(tileWSize * 18,  tileHSize * 13);
    TileWeiRT_point =       iPointMake(tileWSize * 27,  tileHSize * 13);

    LoadBtn_point =         iPointMake(tileWSize * 34,  tileHSize);
    SaveBtn_point =         iPointMake(tileWSize * 34,  tileHSize * 3);
    ExitBtn_point =         iPointMake(tileWSize * 34, tileHSize * 5);
    TileSelRT_point =       iPointMake(tileWSize * 34,  tileHSize * 7);
    selectedImgRT_point =   iPointMake(tileWSize * 36 - tileWSize/2,  tileHSize * 8);
    selectedWeiRT_point =   iPointMake(tileWSize * 36 - tileWSize/2, tileHSize * 10);

    EditRT = new iRect[tileW * tileH];

    for (int i = 0; i < tileW * tileH; i++)
    {
        int x = i % tileW, y = i/tileW;
        EditRT[i] = iRectMake(EditRT_point.x + x * tileWSize, EditRT_point.y + y * tileHSize, tileWSize, tileHSize);
    }
   

    E_RT =          iRectMake(0, 0, tileWSize * 16, tileHSize * 12);
    W_RT =          iRectMake(TileWeiRT_point.x, TileWeiRT_point.y, tileWSize * 8, tileHSize * 8);
    TileRT =        iRectMake(TileRT_point.x, TileRT_point.y, tileWSize * 16, tileHSize * 12);
    TileImgRT =     iRectMake(TileImgRT_point.x, TileImgRT_point.y, tileWSize * 8, tileHSize * 8);
    TileImgRT2 =    iRectMake(TileImgRT2_point.x, TileImgRT2_point.y, tileWSize * 8, tileHSize * 8);
    TileImgRT3 =    iRectMake(TileImgRT3_point.x, TileImgRT3_point.y, tileWSize * 8, tileHSize * 8);
    TileWeiRT =     iRectMake(TileWeiRT_point.x, TileWeiRT_point.y, tileWSize * 8, tileHSize * 8);
    TileSelRT =     iRectMake(TileSelRT_point.x, TileSelRT_point.y, tileWSize * 4, tileHSize * 5);
    LoadBtn =       iRectMake(LoadBtn_point.x, LoadBtn_point.y, tileWSize * 4, tileHSize);
    SaveBtn =       iRectMake(SaveBtn_point.x, SaveBtn_point.y, tileWSize * 4, tileHSize);
    ExitBtn =       iRectMake(ExitBtn_point.x, ExitBtn_point.y, tileWSize * 4, tileHSize);
    selectedImgRT = iRectMake(selectedImgRT_point.x, selectedImgRT_point.y, tileWSize, tileHSize);
    selectedWeiRT = iRectMake(selectedWeiRT_point.x, selectedWeiRT_point.y, tileWSize, tileHSize);

    WeiRT = new iRect[11]; //0~9, INF

    for (int i = 0; i < 11; i++)
    {
        WeiRT[i] = iRectMake(TileWeiRT_point.x + (i % 3) * tileWSize * 2, TileWeiRT_point.y + (i / 3) * tileHSize * 2, tileWSize * 2, tileHSize * 2);
    }

    tEditor = new MapEditor();
    tEditor->init(tileW, tileH, tileWSize, tileHSize);
  
    //set_check = true;
}

void loadMap()
{
    
    ImgPath[0] = "assets/Image/Tile1.bmp";
    ImgPath[1] = "assets/Image/Tile2.bmp";
    ImgPath[2] = "assets/Image/Tile3.bmp";

    texs = new Texture **[3];
    texs[0] = createImageDivide(8, 32, ImgPath[0]);
    texs[1] = createImageDivide(8, 32, ImgPath[1]);
    texs[2] = createImageDivide(8, 32, ImgPath[2]);

    //if (!set_check)
    RTset();
    
    //to do...
    //가중치 처리해줘야함. 아직 안해준것들이 좀 있음. 
    tEditor;

    tmpTileTex = new Texture * [tileW * tileH];
    tmpTileWei = new int [tileW * tileH];
    for (int i = 0; i < tileW * tileH; i++)
    {
        tmpTileTex[i] = NULL;
        tmpTileWei[i] = 0;
    }
}



void drawMap(float dt)
{
    clearRect();
    //===========================================
    //draw iRect
    //===========================================
    int i;
    setRGBA(1, 0, 0, 1);
    for(i=0;i<tileW * tileH;i++)
        drawRect(EditRT[i]);
   
    drawRect(TileRT);
    drawRect(TileImgRT);
    drawRect(TileImgRT2);
    drawRect(TileImgRT3);
    drawRect(TileWeiRT);
    drawRect(TileSelRT);
    drawRect(LoadBtn);
    drawRect(SaveBtn);
    drawRect(ExitBtn);
    drawRect(selectedImgRT);
    drawRect(selectedWeiRT);
    for (i = 0; i < 11; i++)
        drawRect(WeiRT[i]);
    setRGBA(1, 1, 1, 1);

    //===========================================
    //draw string
    //===========================================
    //to do...
   
    setStringName("굴림");
    setStringSize(30);
    setStringRGBA(1, 0, 0, 1);
    setStringBorder(0.5f);
    setStringBorderRGBA(0, 0, 0, 1);
    //test
    for (int i = 0; i < 11; i++)
        drawString(TileWeiRT_point.x + (i%3) * tileWSize * 2, TileWeiRT_point.y + (i / 3) * tileHSize * 2, TOP | LEFT, weight[i]);

    //#need update 위치조정
    const char* loadStr = "Load";
    const char* saveStr = "Save";
    const char* exitStr = "Exit";
    drawString(LoadBtn_point.x, LoadBtn_point.y, TOP | LEFT, loadStr);
    drawString(SaveBtn_point.x, SaveBtn_point.y, TOP | LEFT, saveStr);
    drawString(ExitBtn_point.x, ExitBtn_point.y, TOP | LEFT, exitStr);

    //선택한 가중치
    drawString(selectedWeiRT_point.x, selectedWeiRT_point.y, TOP|LEFT, selWei);

    //===========================================
    //draw tmpTile
    //===========================================
    for (int i = 0; i < tileW * tileH; i++)
    {
        if (tmpTileTex[i])
        {
            drawImage(tmpTileTex[i], i % tileW * tileWSize, i / tileW * tileHSize, TOP | LEFT);
            sprintf(tmpWei, "%d", tmpTileWei[i]);
            drawString(i % tileW * tileWSize, i / tileW * tileHSize, TOP | LEFT, tmpWei);
        }
    }

    //===========================================
    //draw Tile
    //===========================================
    //tEditor->draw(dt, TileRT_point);
    int x = tEditor->tileX;
    int y = tEditor->tileY;
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < x * y; i++)
        {
            if (tEditor->tileIndex[j][i] > -1)
                drawImage(texs[j][tEditor->tileIndex[j][i]], TileRT_point.x + i%x*tileWSize, TileRT_point.y + i/x*tileHSize, TOP | LEFT);
        }
    }

    if (selectedTex)
        drawImage(selectedTex, selectedImgRT_point.x, selectedImgRT_point.y, TOP | LEFT);

    //===========================================
    //draw TileImg 1, 2, 3 & TileWeight 
    //===========================================
    if(selectedTex)
        drawImage(selectedTex, selectedImgRT_point.x, selectedImgRT_point.y, TOP|LEFT);

    //TileImgRT
    setClip(0, tileHSize * 13, tileWSize * 8, tileHSize * 8);
    for (i = 0; i < 256; i++)
        drawImage(texs[0][i], TileImgRT_point.x + (i % 8) * tileWSize, TileImgRT_point.y + (i / 8) * tileHSize, TOP | LEFT);
    setClip(0, 0, 0, 0);

    //TileImgRT2
    setClip(tileWSize * 9, tileHSize * 13, tileWSize * 8, tileHSize * 8);
    for (i = 0; i < 256; i++)
        drawImage(texs[1][i], TileImgRT2_point.x + (i % 8) * tileWSize, TileImgRT2_point.y + (i / 8) * tileHSize, TOP | LEFT);
    setClip(0, 0, 0, 0);

    //TileImgRT3
    setClip(tileWSize * 18, tileHSize * 13, tileWSize * 8, tileHSize * 8);
    for (i = 0; i < 256; i++)
        drawImage(texs[2][i], TileImgRT3_point.x + (i % 8) * tileWSize, TileImgRT3_point.y + (i / 8) * tileHSize, TOP | LEFT);
    setClip(0, 0, 0, 0);

}

void freeMap()
{

    int i, j;

    //Map3.cpp 전역변수 초기화
    selectedTex = NULL;
    selectedWei = 99;

    for (i = 0; i < 10; i++)
    {
        selWei[i] = NULL;
        tmpWei[i] = NULL;
    }

    delete EditRT;

    for (j = 0; j < 3; j++)
    {
        for (i = 0; i < 256; i++)
            freeImage(texs[j][i]);
        delete texs[j];
    }
    delete texs;

    delete tmpTileTex;
    delete tmpTileWei;

    //객체 초기화
    delete tEditor;
}

void containTileImg(iPoint point, iPoint off)
{
    iRect texRT;
   
    if (movingTileImg)
        texs_idx = 0;
    else if (movingTileImg2)
        texs_idx = 1;
    else //if(movingTileImg3)
        texs_idx = 2;

    for (int i = 0; i < 256; i++)
    {
        texRT = iRectMake(off.x + (i % 8) * tileWSize, off.y + (i / 8) * tileHSize, tileWSize, tileHSize);
        if (containPoint(point, texRT))
        {
            selectedTex = texs[texs_idx][i];
            //선택 영역의 이미지 인덱스를 객체 멤버변수에 저장
            tEditor->selectedTile = i;
            
        }
    }
}

void containWeiImg(iPoint point, iPoint off)
{
    iRect texWei;

    for (int i = 0; i < 11; i++)
    {
        texWei = iRectMake(off.x + (i % 3) * tileWSize * 2, off.y + (i / 3) * tileHSize * 2, tileWSize * 2, tileHSize * 2);
        if (containPoint(point, texWei))
        {
            selectedWei = i ;
        }
    }

    sprintf(selWei, "%d", selectedWei);
    tEditor->selectedWeight = selectedWei;
}

void keyMap(iKeyStat stat, iPoint point)
{
    if (stat == iKeyStatBegan)
    {

        //편집영역에 있을때 (격자무늬)
        if (containPoint(point, E_RT))
        {
            if (selWei[0] && selectedTex) //이미지, 가중치 둘다 선택됬을때
            {
                //tEditor->selectedTile = selectedTex
                tEditor->insert(point, texs_idx);
                
                for (int i = 0; i < tileW * tileH; i++)
                {
                    if (containPoint(point, EditRT[i]))
                    {
                        tmpTileTex[i] = selectedTex;
                        tmpTileWei[i] = selectedWei;
                    }

                }
            }
        }

        prevPosition = point;
        //타일 이미지 내
        if (containPoint(point, TileImgRT))
        {
            movingTileImg = true;
            containTileImg(point, TileImgRT_point);
        }
        else if (containPoint(point, TileImgRT2))
        {
            movingTileImg2 = true;
            containTileImg(point, TileImgRT2_point);
        }
        else if (containPoint(point, TileImgRT3))
        {
            movingTileImg3 = true;
            containTileImg(point, TileImgRT3_point);
        }

        //Wei 버튼 (숫자버튼)
        if (containPoint(point, W_RT))
        {
            containWeiImg(point, TileWeiRT_point);
        }


        //load 버튼
        if (containPoint(point, LoadBtn))
        {

            tEditor->load("map.tile");
#ifdef WIN32
            MessageBox(NULL, TEXT("로드완료"), TEXT("Load"), MB_OK);
#endif //WIN32
        }

        //save 버튼
        if (containPoint(point, SaveBtn))
        {
            tEditor->save("map.tile");
#ifdef WIN32
            MessageBox(NULL, TEXT("저장완료"), TEXT("Save"), MB_OK);
#endif //WIN32
        }

        //Exit 버튼
        if (containPoint(point, ExitBtn))
        {
            setLoading(gs_intro, freeMap, loadIntro);
        }

    }
    else if (stat == iKeyStatMoved)
    {
        if (movingTileImg)
        {
            iPoint mp = point - prevPosition;
            prevPosition = point;

            TileImgRT_point.y += mp.y;
            if (TileImgRT_point.y < tileHSize * (tileH + 1) - tileHSize * 24)
                TileImgRT_point.y = tileHSize * (tileH + 1) - tileHSize * 24;
            else if (TileImgRT_point.y > tileHSize * (tileH + 1))
                TileImgRT_point.y = tileHSize * (tileH + 1);
        }
        else if (movingTileImg2)
        {
            iPoint mp = point - prevPosition;
            prevPosition = point;

            TileImgRT2_point.y += mp.y;
            if (TileImgRT2_point.y < tileHSize * (tileH + 1) - tileHSize * 24)
                TileImgRT2_point.y = tileHSize * (tileH + 1) - tileHSize * 24;
            else if (TileImgRT2_point.y > tileHSize * (tileH + 1))
                TileImgRT2_point.y = tileHSize * (tileH + 1);
        }
        else if (movingTileImg3)
        {
            iPoint mp = point - prevPosition;
            prevPosition = point;

            TileImgRT3_point.y += mp.y;
            if (TileImgRT3_point.y < tileHSize * (tileH + 1) - tileHSize * 24)
                TileImgRT3_point.y = tileHSize * (tileH + 1) - tileHSize * 24;
            else if (TileImgRT3_point.y > tileHSize * (tileH + 1))
                TileImgRT3_point.y = tileHSize * (tileH + 1);
        }
    }
    else //if(stat == iKeyStatEnded)
    {
        movingTileImg = false;
        movingTileImg2 = false;
        movingTileImg3 = false;
    }
}
