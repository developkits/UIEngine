// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"
#include <helper/SAdapterBase.h>

#include <time.h>
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")

class CTestAdapterFix : public SAdapterBase , public ICVSelChangedHandler
{
public:
struct UserInfo{
    SStringT strName;
    SStringT strAccount;
    int      iIcon;
};

protected:
   
    SArray<UserInfo>    m_userInfo;
public:
    CTestAdapterFix()
    {
        UserInfo ui[]=
        {//SOUI管理员列表
            {
                _T("想象元素"),
                    _T("100"),
                    0
            },
            {
                _T("启程软件"),
                    _T("101"),
                    1
            },
            {
                _T("L.K"),
                    _T("102"),
                    2
            },
            {
                _T("多点WIFI"),
                    _T("103"),
                    3
            },
            {
                _T("ID-1008"),
                    _T("104"),
                    4
            },
            {
                _T("小可"),
                    _T("105"),
                    5
            },
            {
                _T("任明星"),
                    _T("106"),
                    6
            },
        };
        for(int i=0;i< ARRAYSIZE(ui);i++)
        {
            m_userInfo.Add(ui[i]);
        }
    }
    virtual int getCount()
    {
        return m_userInfo.GetCount();
    }   

    virtual void getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
    {
        if(pItem->GetChildrenCount()==0)
        {
            pItem->InitFromXml(xmlTemplate);
        }
        SImageWnd *pAvatar = pItem->FindChildByID2<SImageWnd>(1);
        pAvatar->SetIcon(m_userInfo.GetAt(position).iIcon);
        SWindow *pName = pItem->FindChildByID(2);
        pName->SetWindowText(m_userInfo.GetAt(position).strName);
        SWindow *pAccount = pItem->FindChildByID(3);
        pAccount->SetWindowText(m_userInfo.GetAt(position).strAccount);
        SWindow *pBtn = pItem->FindChildByID(4);
        pBtn->SetUserData(position);
        pBtn->GetEventSet()->subscribeEvent(EVT_CMD,Subscriber(&CTestAdapterFix::OnButtonClick,this));
    }

    bool OnButtonClick(EventArgs *pEvt)
    {
        SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
        int iItem = pBtn->GetUserData();
        STRACE(SStringT().Format(_T("button of %d item was clicked"),iItem));
        m_userInfo.RemoveAt(iItem);
        this->notifyDataSetChanged();
        return true;
    }
    
    SStringT getItemDesc(int position)
    {
        return m_userInfo.GetAt(position).strName;
    }
    
    virtual BOOL onLVSelChanged(EventLVSelChanged *pEvt)
    {
        if(pEvt->hHitWnd)
        {
            SWindow *pSwnd = SWindowMgr::GetWindow(pEvt->hHitWnd);
            if(pSwnd && pSwnd->GetID() == 4)
            {//id of 4 is to remove the click item
                pEvt->bCancel = TRUE;
            }
            return pEvt->bCancel;
        }else
        {
            return FALSE;
        }
    }
    
    UserInfo getItem(int position)
    {
        SASSERT(position>=0 && position < m_userInfo.GetCount());
        return m_userInfo[position];
    }
};


CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited=FALSE;
} 

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	// 		MARGINS mar = {5,5,30,5};
	// 		DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	SetMsgHandled(FALSE);
	return 0;
}

void CMainDlg::OnShowWindow( BOOL bShow, UINT nStatus )
{
	if(bShow)
	{
 		AnimateHostWindow(200,AW_CENTER);
	}
}

struct flashtime{
    int timeEnd;
    wchar_t szFlash[50];
}g_flashs[]=
{
    {0, L"morning.swf"},
    {8,L"noon.swf"},
    {12,L"afternoon.swf"},
    {18,L"night.swf"}
};

BOOL CMainDlg::OnInitDialog( HWND hWnd, LPARAM lParam )
{
    m_bLayoutInited=TRUE;
    SFlashCtrl * pFlash = FindChildByName2<SFlashCtrl>(L"flash_bkgnd");
    if(pFlash)
    {
        wchar_t szCurDir[MAX_PATH+1];
        GetCurrentDirectoryW(MAX_PATH,szCurDir);

        time_t long_time;
        time( &long_time );
        struct tm * t=localtime(&long_time);

        wchar_t szFlash[MAX_PATH];
        for(int i=3;i>=0;i--)
        {
            if(t->tm_hour>=g_flashs[i].timeEnd)
            {
                swprintf(szFlash,L"%s\\flash\\%s",szCurDir,g_flashs[i].szFlash);
                break;
            }
        }

        pFlash->Play(szFlash);
    }
    
    SComboView *pComboView = FindChildByName2<SComboView>(L"cbx_account");
    if(pComboView)
    {
        SListView *pLstView = pComboView->GetListView();
        CTestAdapterFix *pAdapter = new CTestAdapterFix;
        pLstView->SetAdapter(pAdapter);
        pComboView->SetCVSelChangedHandler(pAdapter);
        pAdapter->Release();
        pComboView->SetCurSel(1);
    }
    return 0;
}

void CMainDlg::OnBtnSetting()
{
    STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_main");
    if(pTab)
    {
        STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
        if(pTurn3d)
        {
            pTurn3d->Turn(pTab->GetPage(_T("login")),pTab->GetPage(_T("settings")),FALSE);
        }
    }
}

void CMainDlg::OnBtnSettingOK()
{
    STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_main");
    if(pTab)
    {
        STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
        if(pTurn3d)
        {
            pTurn3d->Turn(pTab->GetPage(_T("settings")),pTab->GetPage(_T("login")),TRUE);
        }
    }
}

void CMainDlg::OnBtnSettingCancel()
{
    STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_main");
    if(pTab)
    {
        STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
        if(pTurn3d)
        {
            pTurn3d->Turn(pTab->GetPage(_T("settings")),pTab->GetPage(_T("login")),TRUE);
        }
    }
}

BOOL CMainDlg::OnTurn3D( EventArgs *pEvt )
{
    EventTurn3d *pEvt3dTurn = (EventTurn3d*)pEvt;
    STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_main");
    if(pEvt3dTurn->bTurn2Front_)
    {
        pTab->SetCurSel(_T("login"));
    }else
    {
        pTab->SetCurSel(_T("settings"));
    }
    return TRUE;
}

BOOL CMainDlg::OnUserChanged(EventArgs *pEvt)
{
    EventCBSelChange *pEvt2 = sobj_cast<EventCBSelChange>(pEvt);
    SASSERT(pEvt2);
    SImageWnd *pAvatar = FindChildByName2<SImageWnd>(L"img_avatar");
    SASSERT(pAvatar);
    if(pEvt2->nCurSel!=-1)
    {
        SComboView *pComboView = sobj_cast<SComboView>(pEvt->sender);
        CTestAdapterFix *pAdapter = (CTestAdapterFix*)pComboView->GetListView()->GetAdapter();
        CTestAdapterFix::UserInfo ui=pAdapter->getItem(pEvt2->nCurSel);
        pAvatar->SetIcon(ui.iIcon);
    }else
    {
        pAvatar->SetIcon(0);
    }
    return TRUE;
}
