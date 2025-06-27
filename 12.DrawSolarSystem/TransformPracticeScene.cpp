#include "pch.h"
#include "InputManager.h"
#include "D2DTransform.h"
#include "SolarSystemRenderer.h"
#include "TransformPracticeScene.h"

using TestRenderer = myspace::D2DRenderer;
using Vec2 = MYHelper::Vector2F;

// �׼�, �༺�� �ش� Ŭ���� ��� ���� �� ����, (�ʿ�� �Ļ� Ŭ���� �и�)
class Celestial {
private:
    Celestial() = delete;
    Celestial(const Celestial&) = delete;
    void operator=(const Celestial&) = delete;
    D2D1_RECT_F s_rect;

    D2DTM::Transform  m_transform;
    MAT3X2F m_renderTM;
    D2D1_RECT_F m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f); //pivot ���ÿ�
    std::wstring m_name = L"";

    bool m_isSelected = false;
    bool m_isLeader = false;

    bool m_isSelfRotation = true; //

    ComPtr<ID2D1Bitmap1> m_BitmapPtr;

    static int s_id;

public:
    //parameter : bitmap
    // constructor
    Celestial(ComPtr<ID2D1Bitmap1>& bitmap, const D2D1_RECT_F& rectSize ) {
        m_BitmapPtr = bitmap;

        ++s_id;
        m_name += std::to_wstring(s_id);

        m_renderTM = MYTM::MakeRenderMatrix(true);           /// �� ����
        D2D1_SIZE_F size = { m_rect.right - m_rect.left, m_rect.bottom - m_rect.top }; //�簢�� ǥ�ÿ�
        s_rect = rectSize;
        m_transform.SetPivotPreset(D2DTM::PivotPreset::Center, size); //�簢�� ǥ�ÿ� �ǹ�?
    }
    //defualt destructor
    ~Celestial() = default;

    void Update(float deltaTime)
    {   //ȸ���� ���
        if (m_isSelfRotation) // ���� flag
        {
            m_transform.Rotate(deltaTime * 56.f); // ����, 
        }
    }


    // �ڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡ�
    void Draw(TestRenderer& testRender, D2D1::Matrix3x2F viewTM)
    {
        //static  D2D1_RECT_F s_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f); //size          //m_rect, s_rect

        D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();

        D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

        D2D1::ColorF boxColor = D2D1::ColorF::LightGray;

        if (m_isLeader) boxColor = D2D1::ColorF::Red;                                  //���ý� �� ����
        else if (m_isSelected) boxColor = D2D1::ColorF::HotPink;

        testRender.SetTransform(finalTM);
        //testRender.DrawRectangle(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom, boxColor);
        //testRender.DrawCircle(0.f, 0.f,2.f, D2D1::ColorF::Red);

        D2D1_RECT_F dest = D2D1::RectF(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom);

        testRender.DrawBitmap(m_BitmapPtr.Get(), dest);
        //testRender.DrawMessage(m_name.c_str(), s_rect.left, s_rect.top, 200, 50, D2D1::ColorF::Black);
    }
    //�ڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡ�

    void SetScale(const float& rate) {
        m_transform.SetScale({rate,rate});
    }

    void SetPosition(const Vec2& pos) // ������ġ�� �Ķ���� ����
    {
        m_transform.SetPosition(pos);
    }

    void Move(const Vec2& offset)
    {
        m_transform.Translate(offset);
    }

    void Rotate(float angle)
    {
        m_transform.Rotate(angle);
    }

    void ToggleSelected()
    {
        m_isSelected = !m_isSelected;                          //�ʿ� �� ���
    }

    bool IsSelected() const
    {
        return m_isSelected;
    }

    void ToggleSelfRotation()
    {
        m_isSelfRotation = !m_isSelfRotation;                  //�ʿ� �� ���
    }

    /*void SetSRect(float left, float top, float right, float bottom) {

        s_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

    }*/

    D2DTM::Transform* GetTransform()
    {
        return &m_transform;
    }


    //�ڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡ�
    void SetParent(Celestial* parent)
    {
        assert(parent != nullptr);

        if (nullptr != m_transform.GetParent())
        {
            // �̹� �θ� �ִٸ� �θ� ���踦 �����մϴ�.
            m_transform.DetachFromParent();
        }

        m_transform.SetParent(parent->GetTransform());
    }

    void DetachFromParent()
    {
        m_transform.DetachFromParent();
    }

    void SetLeader(bool isLeader)
    {
        m_isLeader = isLeader;
    }
    //�ڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡ�

};


int Celestial::s_id = 0; 



TransformPracticeScene::~TransformPracticeScene()
{
    for (auto& box : m_Celestials)
    {
        delete box;
    }
}





void TransformPracticeScene::SetUp(HWND hWnd)
{
    constexpr int defaultGameObjectCount = 10;
    m_hWnd = hWnd;

    SetWindowText(m_hWnd, 
    L"������ �¾�踦 ����� �ּ���. ���� ��Ģ�� ���� �մϴ�. ^^;;"); //Window Title

    /*std::cout << "�¾��� ������ �ؾ� �մϴ�." << std::endl;
    std::cout << "�༺���� ������ �ϸ� ���ÿ� �¿��� ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�."<< std::endl;
    std::cout << "���� ������ �ϸ鼭 ���ÿ� ������ ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�." << std::endl;
    std::cout << "ȸ�� �ӵ��� �����Ӱ� �����ϼ���." << std::endl;*/
     // scene���� �޾Ƽ� ����
   
    RECT rc;
    if (::GetClientRect(hWnd, &rc))
    {
        float w = static_cast<float>(rc.right - rc.left);
        float h = static_cast<float>(rc.bottom - rc.top);

        m_UnityCamera.SetScreenSize(w, h);
    }
    InitSolarSystem(); // õü �ʱ� ��ġ ����
}

void TransformPracticeScene::Tick(float deltaTime)
{   
    // �Է� �̺�Ʈó�� �� ���
    ProcessMouseEvents();
    ProcessKeyboardEvents();

    
    for (auto& box : m_Celestials)
    {
        box->Update(deltaTime);
    }

    //�ڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡ�
    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();

    MAT3X2F renderTM = MYTM::MakeRenderMatrix(true); // ī�޶� ��ġ ������ ��Ʈ����

    MAT3X2F finalTM = renderTM * cameraTM;
    static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();
    wchar_t buffer[128] = L"";
    MYTM::MakeMatrixToString(cameraTM, buffer, 128);

    globalRenderer.RenderBegin();

    globalRenderer.SetTransform(finalTM);

    // ī�޶� ��ġ ǥ��
    //globalRenderer.DrawRectangle(-10.f, 10.f, 10.f, -10.f, D2D1::ColorF::Red);
    globalRenderer.DrawCircle(0.f, 0.f, 2.f, D2D1::ColorF::Red);

    globalRenderer.DrawMessage(buffer, 10.f, 10.f, 100.f, 100.f, D2D1::ColorF::Black);
    

    for (auto& box : m_Celestials)
    {
        box->Draw(globalRenderer, cameraTM);
    }

    globalRenderer.RenderEnd();
}

void TransformPracticeScene::OnResize(int width, int height)
{ 
    m_UnityCamera.SetScreenSize(width, height);
}

void TransformPracticeScene::ProcessKeyboardEvents()
{
  

}

void TransformPracticeScene::InitSolarSystem()
{
    // ���� ����, Object ���� 
    //AddCelestials bitmap index�� ����
    AddCelestials({ 600.f,550.f },0); //0 //�¾�
    m_Celestials[0]->SetScale(1.2f);

    AddCelestials({ 100.f,500.f },1); //1 //����
    m_Celestials[1]->SetScale(1.0f);

    AddCelestials({ 200.f,450.f },2); //2 //��
    m_Celestials[2]->SetScale(0.4f);

    AddCelestials({ 600.f,100.f },3); //3 //���칫
    m_Celestials[3]->SetScale(1.1f);

    AddCelestials({ 600.f,200.f }, 4); //4 //����
    m_Celestials[4]->SetScale(0.7f);
    
    AddCelestials({ 670.f,470.f }, 5); //5 //������ 1ȣ
    m_Celestials[5]->SetScale(0.4f);
    
    m_Celestials[1]->SetParent(m_Celestials[0]); //�¾� - ����

    m_Celestials[3]->SetParent(m_Celestials[0]); //�¾� - �ƿ칫

    m_Celestials[5]->SetParent(m_Celestials[0]); //�¾� - ������1ȣ

    m_Celestials[4]->SetParent(m_Celestials[3]); //�ƿ칫 - ����

    m_Celestials[2]->SetParent(m_Celestials[1]); //���� - ��
    
    
}

void TransformPracticeScene::AddCelestials(D2D1_POINT_2F point, int index) //bitmap index, size
{
    D2D1_RECT_F rectSize;
    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();
    cameraTM.Invert();

    D2D1_POINT_2F worldPt = cameraTM.TransformPoint(point);
    if (index == 0) {
        SolarSystemRenderer::Instance().CreateBitmapFromFile(L"./Resource/Sun.png", *m_BitmapPtr.GetAddressOf());
        rectSize = D2D1::RectF(0.f,0.f,100.f,100.f);
        
    }
    else if (index == 1) {
        //����
        SolarSystemRenderer::Instance().CreateBitmapFromFile(L"./Resource/Earth.png", *m_BitmapPtr.GetAddressOf());
        rectSize = D2D1::RectF(0.f, 0.f, 100.f, 100.f);
    }
    else if (index == 2) {
        //��
        SolarSystemRenderer::Instance().CreateBitmapFromFile(L"./Resource/DeathStar.png", *m_BitmapPtr.GetAddressOf());
        rectSize = D2D1::RectF(0.f, 0.f, 100.f, 100.f);
    }
    else if (index == 3) {
        //���칫�ƹ���
        SolarSystemRenderer::Instance().CreateBitmapFromFile(L"./Resource/Omua.png", *m_BitmapPtr.GetAddressOf());
        rectSize = D2D1::RectF(0.f, 0.f, 100.f, 100.f);
    }
    else if (index == 4) {
        //Falcon
        SolarSystemRenderer::Instance().CreateBitmapFromFile(L"./Resource/Falcon.png", *m_BitmapPtr.GetAddressOf());
        rectSize = D2D1::RectF(0.f, 0.f, 100.f, 100.f);
    }
    else if (index == 5) {
        //Falcon
        SolarSystemRenderer::Instance().CreateBitmapFromFile(L"./Resource/Voyager.png", *m_BitmapPtr.GetAddressOf());
        rectSize = D2D1::RectF(0.f, 0.f, 100.f, 100.f);
    }
    Celestial* pNewBox = new Celestial(m_BitmapPtr,rectSize); //�����ڿ� size �߰�
    pNewBox->SetPosition(Vec2(worldPt.x-50, worldPt.y+50));

    m_Celestials.push_back(pNewBox);
}


void TransformPracticeScene::ClearCelestials()
{


    for (auto& box : m_Celestials)
    {
        delete box;
    }

    m_Celestials.clear();

    m_SelectedBoxObjects.clear();


}

