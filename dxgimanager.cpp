#include "dxgimanager.h"
#include <QDebug>
#include<QCursor>
Texture::~Texture()
{

    //上一回copyImage已经relase过了，所以m_texture不是空指针但不能再relase,copyimage 81行m_texture->relase以后令m_texture=nullptr问题解决
    if(m_texture!=nullptr){
        m_texture->Release();
        m_texture=nullptr;
    }

}

DxgiTextureStaging::DxgiTextureStaging(ID3D11Device *device, ID3D11DeviceContext *context)
    : m_device(device), m_context(context)
{

}

DxgiTextureStaging::~DxgiTextureStaging()
{

    m_device->Release();
    m_device=nullptr;
    m_context->Release();
    m_context=nullptr;
}
//#define MOVE_TEST
//#define UNIQUEPTR

QPixmap DxgiTextureStaging::copyToImage(IDXGIResource *res)
{

    D3D11_TEXTURE2D_DESC desc;
    ID3D11Texture2D *textrueRes = nullptr;
    HRESULT hr = res->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&textrueRes));
    //QPixmap pixmap;
#ifdef MOVE_TEST
    std::shared_ptr<QPixmap> pixmap=std::make_shared<QPixmap>();
    //QPainter p(&pixmap);
#endif
    if (FAILED(hr)) {
       qDebug() << "Failed to ID3D11Texture2D result =" << hex << uint(hr);

#ifdef MOVE_TEST
       return pixmap;
#else
       return QPixmap();
#endif
    }
    textrueRes->GetDesc(&desc);

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = desc.Width;
    texDesc.Height = desc.Height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_STAGING;
    texDesc.Format = desc.Format;
    texDesc.BindFlags = 0;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    texDesc.MiscFlags = 0;
    m_device->CreateTexture2D(&texDesc, nullptr, &m_texture);
    m_context->CopyResource(m_texture, textrueRes);



    IDXGISurface1 *surface = nullptr;
    hr = m_texture->QueryInterface(__uuidof(IDXGISurface1), reinterpret_cast<void **>(&surface));
    if (FAILED(hr)) {
       qDebug() << "Failed to QueryInterface IDXGISurface1 ErrorCode =" << hex << uint(hr);
#ifdef MOVE_TEST
       return pixmap;
#else
       return QPixmap();
#endif
    }



    DXGI_MAPPED_RECT map;
    surface->Map(&map, DXGI_MAP_READ);     //耗时大头在这  十几毫秒






#ifdef MOVE_TEST
    auto pix = QPixmap::fromImage(QImage(static_cast<uchar *>(map.pBits),                //这一步需要1ms-2ms
                                       int(desc.Width), int(desc.Height), QImage::Format_ARGB32));
    pixmap->swap(pix);
    //pixmap->swap(QPixmap::fromImage(QImage(static_cast<uchar *>(map.pBits),int(desc.Width), int(desc.Height), QImage::Format_ARGB32)));
    //pixmap.save(QString("D:/firecv/%1.jpg").arg(curPix++),"JPG");
#else
         auto beforeTime = std::chrono::steady_clock::now();
    //QPixmap pixmap;
    //QPixmap::fromImage(QImage(static_cast<uchar *>(map.pBits),int(desc.Width), int(desc.Height), QImage::Format_ARGB32)).swap(pixmap)
            QPixmap pixmap=QPixmap::fromImage(QImage(static_cast<uchar *>(map.pBits),int(desc.Width), int(desc.Height), QImage::Format_ARGB32));
         auto afterTime = std::chrono::steady_clock::now();
         double duration_millsecond = std::chrono::duration<double, std::milli>(afterTime - beforeTime).count();
             qDebug() <<"copyimage所需时间"<< duration_millsecond << "毫秒" << endl;
#endif



    surface->Unmap();
    surface->Release();
    surface=nullptr;
    m_texture->Release();
    m_texture=nullptr;


#if 1

    QPainter painter(&pixmap);//绘制鼠标
    CURSORINFO ci;
    memset(&ci, 0, sizeof(ci));
    ci.cbSize = sizeof(ci);
    if (GetCursorInfo(&ci))
    {
                HICON hIcon = CopyIcon(ci.hCursor);
                if (hIcon)
                {
                    ICONINFO ii;
                    if (GetIconInfo(hIcon, &ii))
                    {
                        QPixmap mp1=QtWin::fromHBITMAP(ii.hbmColor);
                        mp1.setMask(QBitmap::fromImage(QtWin::fromHBITMAP(ii.hbmMask).toImage()));//给图像设置掩码实现不该画的区域透明透明
                        if(mp1.isNull()){
                            //qDebug()<<"没捕获到鼠标图像"<<endl;
                            mp1=preCursor;//解决未捕获到图像该帧鼠标消逝的问题，实际是鼠标移动时某一帧捕获到了但是过去的太快了
                        }else{
                            preCursor=mp1;
                        }
                        painter.drawPixmap(ci.ptScreenPos.x,ci.ptScreenPos.y,mp1);
                        DeleteObject(ii.hbmColor);
                        DeleteObject(ii.hbmMask);
                    }
                    DestroyIcon(hIcon);
                }
    }
#endif
//应该是没有调用右值引用

    return pixmap;
}
#if 0
QPixmap DxgiTextureStaging::DrawCursor2(D3D11_MAPPED_SUBRESOURCE mapdesc, D3D11_TEXTURE2D_DESC desc, DXGI_OUTDUPL_FRAME_INFO frameInfo)
{
    HRESULT hRes = S_OK;
    bool bShowCursor = true;


    if (mapdesc.pData)
    {

        CURSORINFO ci;
        memset(&ci, 0, sizeof(ci));
        ci.cbSize = sizeof(ci);

        if (GetCursorInfo(&ci))
        {
            memcpy(&m_CursorPos, &ci.ptScreenPos, sizeof(m_CursorPos));

            if (ci.flags & CURSOR_SHOWING)
            {
                if (ci.hCursor != m_hCurrentCursor) // re-get cursor data
                {
                    HICON hIcon = CopyIcon(ci.hCursor);
                    m_hCurrentCursor = ci.hCursor;

                    //free(m_CursorData);
                    //m_CursorData = NULL;

                    if (hIcon)
                    {
                        ICONINFO ii;
                        if (GetIconInfo(hIcon, &ii))
                        {
                            xHotspot = int(ii.xHotspot);
                            yHotspot = int(ii.yHotspot);

                            //m_CursorData = GetCursorData(hIcon, ii, m_CursorWidth, m_CursorHeight, m_CursorPitch);

                            //DXGI_MAPPED_RECT map;
                            //surface->Map(&map, DXGI_MAP_READ);
                            //QPixmap pixmap = QPixmap::fromImage(QImage(static_cast<uchar *>(map.pBits),
                                                               //int(desc.Width), int(desc.Height), QImage::Format_ARGB32));

                            //QPixmap pixmap = QPixmap::fromImage(QImage(static_cast<uchar *>(),)
                            QPixmap mp=QtWin::fromHBITMAP(ii.hbmMask);

                            DeleteObject(ii.hbmColor);
                            DeleteObject(ii.hbmMask);
                        }

                        DestroyIcon(hIcon);
                    }
                }
            }
            else
            {
                bShowCursor = false;
            }
        }


//        if (m_CursorData && bShowCursor)
//        {
//            // Not supporting mono and masked pointers at the moment
//            //printf("Drawing pointer at %d %d\n", data->PointerPosition.Position.x, data->PointerPosition.Position.y);
//            const int ptrx = m_CursorPos.x - xHotspot;
//            const int ptry = m_CursorPos.y - yHotspot;
//            uint8_t* ptr = m_CursorData;
//            uint8_t* dst;
//            // ### Should really do the blending on the GPU (Using DirectX) rather than using SSE2 on the CPU
//            const int ptrw = min(m_CursorWidth, desc.Width - ptrx);
//            for (unsigned int y = 0; y < m_CursorHeight; ++y)
//            {
//                if (y + ptry >= desc.Height)
//                    break;
//                dst = static_cast<uint8_t*>(mapdesc.pData) + (((y + ptry) * mapdesc.RowPitch) + (ptrx * 4));
//                //memcpy(dst, ptr, data->PointerShape.Width * 4);
//                ARGBBlendRow_SSE2(ptr, dst, dst, ptrw);
//                ptr += m_CursorPitch;
//            }
//        }
    }

    return QPixmap();
}
#endif


DxgiManager::DxgiManager()
{

}

DxgiManager::~DxgiManager()
{
    m_duplication->Release();
    m_duplication=nullptr;
}

bool DxgiManager::init(int selectAdapt)
{
    firstAdapte=selectAdapt;
    ID3D11Device *d3dDevice = nullptr;
    ID3D11DeviceContext *d3dContext = nullptr;
    D3D_FEATURE_LEVEL feat = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &d3dDevice, &feat, &d3dContext);
    if (FAILED(hr)) {
        m_lastError = "Failed to D3D11CreateDevice ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIDevice *dxgiDevice = nullptr;
    hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if(FAILED(hr)) {
        m_lastError = "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }


#if 1

    IDXGIFactory * pFactory = NULL;

    CreateDXGIFactory(__uuidof(IDXGIFactory) ,(void**)&pFactory);

    UINT i = 0;
    IDXGIAdapter * pAdapter;
    std::vector <IDXGIAdapter*> vAdapters;
    while(pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        vAdapters.push_back(pAdapter);
        ++i;
    }

    if(pFactory)
        {
            pFactory->Release();
        }

#endif
//    for(int i=0;i<vAdapters.size();++i){
//        qDebug()<<vAdapters[i]->
//    }
    //qDebug()<<"有几块显卡:"<<vAdapters.size()<<endl;
    int tmp=selectAdapt;//0是630，1是1660ti //2是微软图形驱动
    DXGI_ADAPTER_DESC pDesc;
    vAdapters[tmp]->GetDesc(&pDesc);
    qDebug()<<"显卡信息描述："<<pDesc.Description<<endl;
    for(int i=0;pDesc.Description[i]&&i<128;++i){
        qDebug()<<(char)pDesc.Description[i];
    }
    //IDXGIAdapter *dxgiAdapter = nullptr;
    //hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&vAdapters[tmp]));
    dxgiDevice->Release();
    if (FAILED(hr)) {
        m_lastError = "Failed to Get IDXGIAdapter ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    IDXGIOutput *dxgiOutput = nullptr;
    QVector<IDXGIOutput *> outputs;
    //for(uint i = 0; dxgiAdapter->EnumOutputs(i, &dxgiOutput) != DXGI_ERROR_NOT_FOUND; ++i) {
    for(uint i = 0; vAdapters[tmp]->EnumOutputs(i, &dxgiOutput) != DXGI_ERROR_NOT_FOUND; ++i) {
        outputs.push_back(dxgiOutput);
    }
    //dxgiAdapter->Release();
    vAdapters[tmp]->Release();
#if 0
    //看看是哪块视频卡
    DXGI_OUTPUT_DESC oDesc;
    qDebug()<<"output sz:"<<outputs.size()<<endl;  //就一块
    outputs[0]->GetDesc(&oDesc);
    for(int i=0;oDesc.DeviceName[i]&&i<32;++i){
        qDebug()<<(char)oDesc.DeviceName[i];
    }
    qDebug()<<endl;
#endif
    if (outputs.size() > 0) dxgiOutput = outputs.at(0);
    else {
        m_lastError = "Failed to IDXGIOutput is Empty!";
        return false;
    }

    IDXGIOutput6 *dxgiOutput6 = nullptr;
    hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput6), reinterpret_cast<void**>(&dxgiOutput6));
    dxgiOutput->Release();
    if (FAILED(hr)) {
        m_lastError = "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    hr = dxgiOutput6->DuplicateOutput(d3dDevice, &m_duplication);

    dxgiOutput6->Release();
    dxgiOutput6=nullptr;
    if (FAILED(hr)) {
        m_lastError = "Failed to DuplicateOutput ErrorCode = " + QString::number(uint(hr), 16);
        return false;
    }

    DXGI_OUTDUPL_DESC desc;
    m_duplication->GetDesc(&desc);
    m_texture = new DxgiTextureStaging(d3dDevice, d3dContext);
    if (desc.DesktopImageInSystemMemory) {
        qDebug() << "Desc: CPU shared with GPU";
    } else {
        qDebug() << "Desc: CPU not shared with GPU";
    }

    return true;
}
void DxgiManager::reDuplication(){
    if(this->m_texture){
        delete this->m_texture;
        m_texture=nullptr;
    }



    ID3D11Device *d3dDevice = nullptr;
    ID3D11DeviceContext *d3dContext = nullptr;
    D3D_FEATURE_LEVEL feat = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &d3dDevice, &feat, &d3dContext);

    if (FAILED(hr)) {
        m_lastError = "Failed to D3D11CreateDevice ErrorCode = " + QString::number(uint(hr), 16);
        return ;
    }

    IDXGIDevice *dxgiDevice = nullptr;
    hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if(FAILED(hr)) {
        m_lastError = "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
        return ;
    }


#if 1

    IDXGIFactory * pFactory = NULL;

    CreateDXGIFactory(__uuidof(IDXGIFactory) ,(void**)&pFactory);

    UINT i = 0;
    IDXGIAdapter * pAdapter;
    std::vector <IDXGIAdapter*> vAdapters;
    while(pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        vAdapters.push_back(pAdapter);
        ++i;
    }

    if(pFactory)
        {
            pFactory->Release();
        }

#endif
//    for(int i=0;i<vAdapters.size();++i){
//        qDebug()<<vAdapters[i]->
//    }
    //qDebug()<<"有几块显卡:"<<vAdapters.size()<<endl;
    int tmp=firstAdapte;//0是630，1是1660ti //2是微软图形驱动
    DXGI_ADAPTER_DESC pDesc;
    vAdapters[tmp]->GetDesc(&pDesc);
    qDebug()<<"显卡信息描述："<<pDesc.Description<<endl;
    for(int i=0;pDesc.Description[i]&&i<128;++i){
        qDebug()<<(char)pDesc.Description[i];
    }
    //IDXGIAdapter *dxgiAdapter = nullptr;
    //hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&vAdapters[tmp]));
    dxgiDevice->Release();
    if (FAILED(hr)) {
        m_lastError = "Failed to Get IDXGIAdapter ErrorCode = " + QString::number(uint(hr), 16);
        return ;
    }

    IDXGIOutput *dxgiOutput = nullptr;
    QVector<IDXGIOutput *> outputs;
    //for(uint i = 0; dxgiAdapter->EnumOutputs(i, &dxgiOutput) != DXGI_ERROR_NOT_FOUND; ++i) {
    for(uint i = 0; vAdapters[tmp]->EnumOutputs(i, &dxgiOutput) != DXGI_ERROR_NOT_FOUND; ++i) {
        outputs.push_back(dxgiOutput);
    }
    //dxgiAdapter->Release();
    vAdapters[tmp]->Release();

    if (outputs.size() > 0) dxgiOutput = outputs.at(0);
    else {
        m_lastError = "Failed to IDXGIOutput is Empty!";
        return ;
    }
    m_duplication->Release();//注意重新初始化要有这一步

    IDXGIOutput6 *dxgiOutput6 = nullptr;
    hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput6), reinterpret_cast<void**>(&dxgiOutput6));
    dxgiOutput->Release();
    if (FAILED(hr)) {
        m_lastError = "Failed to QueryInterface IDXGIOutput6 ErrorCode = " + QString::number(uint(hr), 16);
        return ;
    }

    hr = dxgiOutput6->DuplicateOutput(d3dDevice, &m_duplication);

    dxgiOutput6->Release();
    if (FAILED(hr)) {
        m_lastError = "Failed to DuplicateOutput ErrorCode = " + QString::number(uint(hr), 16);
        return ;
    }

    DXGI_OUTDUPL_DESC desc;
    m_duplication->GetDesc(&desc);
    m_texture = new DxgiTextureStaging(d3dDevice, d3dContext);
    if (desc.DesktopImageInSystemMemory) {
        qDebug() << "Desc: CPU shared with GPU";
    } else {
        qDebug() << "Desc: CPU not shared with GPU";
    }
    return ;

}
QString DxgiManager::lastError() const
{
    return m_lastError;
}
void DxgiManager::clearLastError(){
    m_lastError.clear();
}
QPixmap DxgiManager::grabScreen()
{
    IDXGIResource *desktopRes;
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    QPixmap im;
    while (true) {//这样相当于不补帧//必须等下层接口更新才返回

        HRESULT hr = m_duplication->AcquireNextFrame(30, &frameInfo, &desktopRes);
        if (FAILED(hr)) {
            m_lastError = "Failed to AcquireNextFrame ErrorCode = " + QString::number(uint(hr), 16);
            qDebug()<<m_lastError<<endl;
            if(uint(hr)==0x887A0026||uint(hr)==0x887A0001){
                try{
                    this->reDuplication();
                    qDebug()<<"重新初始化显卡资源"<<endl;
                }catch(std::exception &e){
                    qDebug()<<e.what()<<endl;
                }
                catch(...){
                    qDebug()<<"初始化失败,等会重新初始化，可能是锁类资源cpu部分占用"<<endl;
                }
                return im;
                //return QImage();
            }
            m_duplication->ReleaseFrame();
            return im;
            //return QImage();
        }
        if (frameInfo.LastPresentTime.QuadPart) break;//值为0说明自上一帧以来没更新，则继续捕捉
        m_duplication->ReleaseFrame();//先释放资源下次获取帧才能成功
    }



    QPixmap mp;
    mp=m_texture->copyToImage(desktopRes);//先保存资源再释放帧，否则保存下来的是黑屏数据  //这一步占总时间一半
    //im=std::move(mp->toImage());
    //static int curPix=0;
    //mp.save(QString("D:/firecv/%1.jpg").arg(curPix++),"JPG");
    //im.save(QString("D:/firecv/%1.jpg").arg(curPix++),"JPG");
    m_duplication->ReleaseFrame();
    //return m_texture->copyToImage(desktopRes);
    return mp;
    //return mp.toImage();
#if 0
    //创建绘画对象
    QPainter painter(&mp); //玄学代码，把pixmap用qPainter构造一下问题解决了，不整截的图会出现黑块//所需时间0.03ms忽略不计





    //auto beforeTime = std::chrono::steady_clock::now();  //鼠标绘制所需时间0.5ms可以忽略

//#define QT_NO_WARNING_OUTPUT
//#define QT_NO_DEBUG_OUTPUT
//绘制鼠标部分
    CURSORINFO ci;
    memset(&ci, 0, sizeof(ci));
    ci.cbSize = sizeof(ci);
    if (GetCursorInfo(&ci))
    {
                HICON hIcon = CopyIcon(ci.hCursor);
                if (hIcon)
                {
                    ICONINFO ii;
                    if (GetIconInfo(hIcon, &ii))
                    {
                        QPixmap mp1=QtWin::fromHBITMAP(ii.hbmColor);
                        mp1.setMask(QBitmap::fromImage(QtWin::fromHBITMAP(ii.hbmMask).toImage()));//给图像设置掩码实现不该画的区域透明透明
                        if(mp1.isNull()){
                            //qDebug()<<"没捕获到鼠标图像"<<endl;
                            mp1=preCursor;//解决未捕获到图像该帧鼠标消逝的问题，实际是鼠标移动时某一帧捕获到了但是过去的太快了
                        }else{
                            preCursor=mp1;
                        }
                        painter.drawPixmap(ci.ptScreenPos.x,ci.ptScreenPos.y,mp1);
                        DeleteObject(ii.hbmColor);
                        DeleteObject(ii.hbmMask);
                    }
                    DestroyIcon(hIcon);
                }
    }
#endif 
}
void DxgiManager::Wchar_tToString(std::string& szDst, wchar_t*wchar)
{
    wchar_t * wText = wchar;
    DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,wText,-1,NULL,0,NULL,FALSE);//WideCharToMultiByte的运用
    char *psText;  // psText为char*的临时数组，作为赋值给std::string的中间变量
    psText = new char[dwNum];
    WideCharToMultiByte (CP_OEMCP,NULL,wText,-1,psText,dwNum,NULL,FALSE);//WideCharToMultiByte的再次运用
    szDst = psText;// std::string赋值
    delete []psText;// psText的清除
}
std::vector<QString> DxgiManager::selectAdapt(){//选择录制显卡

    IDXGIFactory * pFactory = NULL;

    CreateDXGIFactory(__uuidof(IDXGIFactory) ,(void**)&pFactory);

    UINT i = 0;
    IDXGIAdapter * pAdapter;
    std::vector <IDXGIAdapter*> vAdapters;
    while(pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        vAdapters.push_back(pAdapter);
        ++i;
    }

    if(pFactory)
        {
            pFactory->Release();
        }

//    for(int i=0;i<vAdapters.size();++i){
//        qDebug()<<vAdapters[i]->
//    }
    //qDebug()<<"有几块显卡:"<<vAdapters.size()<<endl;
    //int tmp=0;//0是630，1是1660ti //2是微软图形驱动
    DXGI_ADAPTER_DESC pDesc;
    std::vector<QString> ve;
    for(size_t i=0;i<vAdapters.size();++i){
        vAdapters[i]->GetDesc(&pDesc);
        std::string s;
        Wchar_tToString(s,pDesc.Description);
        ve.push_back(QString::fromStdString(s));
    }
//    vAdapters[tmp]->GetDesc(&pDesc);
//    qDebug()<<"显卡信息描述："<<pDesc.Description<<endl;
//    for(int i=0;pDesc.Description[i]&&i<128;++i){
//        qDebug()<<(char)pDesc.Description[i];
//    }
    return ve;
}
