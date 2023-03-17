#ifndef DXGIMANAGER_H
#define DXGIMANAGER_H
#include<QGuiApplication>
#include <QPixmap>
#include<QtWin>
#include<QPainter>
#ifdef Q_OS_WIN
# include <windows.h>
#  if defined (_WIN32_WINNT_WIN10)  //win10 dxgi1_6
#   include <dxgi1_6.h>
#   include <d3d11.h>
#  endif
#endif
#include<QBitmap>
#include<QBuffer>
#include<memory>
class Texture
{
public:
    virtual ~Texture();
    //virtual QPixmap copyToImage(IDXGIResource *res) = 0;
    virtual QPixmap  copyToImage(IDXGIResource *res)=0;
    //virtual HRESULT DrawCursor2(D3D11_MAPPED_SUBRESOURCE mapdesc, D3D11_TEXTURE2D_DESC desc, DXGI_OUTDUPL_FRAME_INFO frameInfo);
protected:
    ID3D11Texture2D *m_texture = nullptr;
};

class DxgiTextureStaging : public Texture
{
public:
    DxgiTextureStaging(ID3D11Device *device, ID3D11DeviceContext *context);
    ~DxgiTextureStaging();

    //QPixmap copyToImage(IDXGIResource *res);
    QPixmap  copyToImage(IDXGIResource *res);
    QPixmap preCursor;//保存前一帧图像
    //QPixmap DrawCursor2(D3D11_MAPPED_SUBRESOURCE mapdesc, D3D11_TEXTURE2D_DESC desc, DXGI_OUTDUPL_FRAME_INFO frameInfo);
public:

    ID3D11Device *m_device = nullptr;
    ID3D11DeviceContext * m_context = nullptr;
};

class DxgiManager
{
public:
    DxgiManager();
    ~DxgiManager();
    int curpixi=0;
    bool init(int selectAdapt);
    QString lastError() const;
    void clearLastError();
    QPixmap grabScreen();
    static std::vector<QString> selectAdapt();//选择录制显卡
    static void Wchar_tToString(std::string& szDst, wchar_t*wchar);
    void reDuplication();
private:
    int firstAdapte=0;

    QPixmap preCursor;//保存前一帧图像

    QString m_lastError = QString();
    Texture *m_texture = nullptr;
    IDXGIOutputDuplication *m_duplication = nullptr;

};

#endif // DXGIMANAGER_H
