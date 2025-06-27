#pragma once
#include "OnlyForTestScene.h"
#include "Camera2D.h"
#include <list>
#include <vector>
#include <wrl/client.h>
#include <d2d1_1.h>

class Celestial;
class TransformPracticeScene : public OnlyForTestScene
{
public:
    TransformPracticeScene() = default;
    virtual ~TransformPracticeScene();
    
    void SetUp(HWND hWnd) override;

    void Tick(float deltaTime) override;

    void OnResize(int width, int height) override;

private:

    void ProcessKeyboardEvents();
    void InitSolarSystem();
    void AddCelestials(D2D1_POINT_2F point,int index);
    void ClearCelestials();

    UnityCamera m_UnityCamera;
    ComPtr<ID2D1Bitmap1> m_BitmapPtr;
    std::vector<Celestial*> m_Celestials;

    std::list<Celestial*> m_SelectedBoxObjects; // 클릭 선택 
};

