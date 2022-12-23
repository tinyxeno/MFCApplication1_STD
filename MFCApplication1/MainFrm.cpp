
// MainFrm.cpp : CMainFrame クラスの実装
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "MFCApplication1Set.h"
#include "MFCApplication1Doc.h"
#include "MFCApplication1View.h"

// CMainFrame

/// <summary>
/// 連想リスト
/// </summary>
typedef struct _tagASSOC
{
	UINT key;
	LPCTSTR value;
}ASSOC, * LPASSOC;

/// <summary>
/// アクセラレータキーのプリフィックス
/// </summary>
ASSOC   m_aControlKey[] =
{
	{   FCONTROL,   _T("Ctrl+") },
	{   FSHIFT,     _T("Shift+")},
	{   FALT,       _T("Alt+")  },
};

/// <summary>
/// アクセラレータキーの固定キーの名称
/// </summary>
ASSOC   m_aValueKey[] =
{
	{   VK_INSERT,  _T("Ins")   },
	{   VK_DELETE,  _T("Del")   },
	{   VK_RETURN,  _T("Enter") },
	{   VK_BACK,    _T("BS")    },
	{   _T('?'),    _T("?")     },
	{   _T('/'),    _T("?")     },
	{   VK_OEM_2,   _T("?")     },
};

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CMainFrame::OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CMainFrame::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_CLOSE, ID_FILE_PAGE_SETUP, &CMainFrame::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_PRINT, ID_FILE_SEND_MAIL, &CMainFrame::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_CLEAR, ID_EDIT_CLEAR_ALL, &CMainFrame::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_FIND, ID_EDIT_FIND, &CMainFrame::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_REPEAT, ID_EDIT_REPLACE, &CMainFrame::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_UNDO, ID_EDIT_REDO, &CMainFrame::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_INSERT_NEW, ID_OLE_VERB_LAST, &CMainFrame::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ROW, &CMainFrame::OnUpdateIndicatorRow)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COL, &CMainFrame::OnUpdateIndicatorCol)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MODIFY, &CMainFrame::OnUpdateIndicatorModify)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DATE, &CMainFrame::OnUpdateIndicatorDate)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TIME, &CMainFrame::OnUpdateIndicatorTime)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DRAWITEM()
	ON_WM_MENUSELECT()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CMainFrame::OnTtnNeedtext)
	ON_UPDATE_COMMAND_UI_RANGE(ID_INDICATOR_CAPS, ID_INDICATOR_SCRL, &CMainFrame::OnUpdateIndicatorCaps)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケーター
	ID_INDICATOR_ROW,
	ID_INDICATOR_COL,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_MODIFY,
	ID_INDICATOR_DATE,
	ID_INDICATOR_TIME,
};

// CMainFrame コンストラクション/デストラクション

CMainFrame::CMainFrame() noexcept
{
	// TODO: メンバー初期化コードをここに追加してください。
	::SecureZeroMemory(&m_systemTime, sizeof(SYSTEMTIME));
	UINT
		source[] = {106,103,102,101,104},
		value = sizeof(source) / sizeof(UINT);
	BOOL result = m_imgSmall.Create(16, 15, ILC_COLOR24 | ILC_MASK, value, 0);
	if (result)
	{
		result = m_imgLarge.Create(32, 32, ILC_COLOR24 | ILC_MASK, value, 0);
		if (result)
		{
			for (UINT index = 0; result && index < value; index++)
			{
				result = FALSE;
				HICON value = ::LoadIcon(nullptr, MAKEINTRESOURCE(source[index]));
				if (nullptr != value && INVALID_HANDLE_VALUE != value)
				{
					result = 0 <= m_imgSmall.Add(value);
					if (result)
					{
						result = 0 <= m_imgLarge.Add(value);
					}
				}
			};
		}
	}
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("ツール バーの作成に失敗しました。\n");
		return -1;      // 作成できない場合
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("ステータス バーの作成に失敗しました。\n");
		return -1;      // 作成できない場合
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: ツール バーをドッキング可能にしない場合は、これらの 3 行を削除してください。
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// ツールバーのボタン名を設定し、ツールバーのビットマップをメニューバーへ設定する
	BOOL result = FALSE;
	// メニューバーを取得
	CMenu* pMenu = GetMenu();
	if (nullptr != pMenu)
	{
		// メニュー情報取得構造体を作成する
		UINT index = sizeof(MENUITEMINFO);
		LPMENUITEMINFO pMenuItemInfo = (LPMENUITEMINFO)::malloc(index);
		if (nullptr != pMenuItemInfo)
		{
			::SecureZeroMemory(pMenuItemInfo, index);
			pMenuItemInfo->cbSize = (UINT)index;
			pMenuItemInfo->fMask = MIIM_STRING;
			result = FALSE;
			int count = 0;
			LPACCEL pAccel = nullptr;
			{
				// アクセラレータを読み込んでメニューバーへ設定する
				// リソースからアクセラレータを取得
				HACCEL hAccel = ::LoadAccelerators(
					::AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
				if (nullptr != hAccel &&
					INVALID_HANDLE_VALUE != hAccel)
				{
					// アクセラレータ情報をメニューバーへ反映する
						// アクセラレータ情報の個数を取得する
					count = ::CopyAcceleratorTable(hAccel, nullptr, 0u);
					if (0 < count)
					{
						// アクセラレータ情報取得用配列を構築する
						index = count * sizeof(ACCEL);
						pAccel = (LPACCEL)::malloc(index);
						if (nullptr != pAccel)
						{
							// アクセラレータ情報を取得する
							result = count == ::CopyAcceleratorTable(hAccel, pAccel, count);
						}// アクセラレータ情報配列の作成に失敗
					}// アクセラレータの個数が 0
					// アクセラレータハンドルの破棄
					::DestroyAcceleratorTable(hAccel);
					hAccel = nullptr;
				}
			}
			if (result && 0 < count && nullptr != pAccel)
			{
				// アクセラレータ情報の個数でループ
				for (int index = 0; index < count; index++)
				{
					// アクセラレータ情報を取得する
					LPACCEL source = &pAccel[index];
					// アクセラレータIDに該当するメニューアイテムに設定されている文字列を取得する
					CString result;
					if (0 < pMenu->GetMenuString(source->cmd, result, MF_BYCOMMAND))
					{
						// アクセラレータ文字列がメニューバーアイテムに既に設定されているかどうかを判定する（先勝ち１個のみ）
						if (-1 == result.Find(_T("\t")))
						{
							// メニューアイテム文字列にアクセラレータ文字列が設定されていない場合
							result += _T("\t");
							// アクセラレータ文字列のプリフィックスを設定 Ctrl,Shift,Alt 最大3個
							size_t value = sizeof(::m_aControlKey) / sizeof(ASSOC);
							for (size_t index = 0; index < value; index++)
							{
								LPASSOC value = &::m_aControlKey[index];
								if (value->key == (source->fVirt & (value->key)))
								{
									result += value->value;
								}
							};
							// A～Z 文字
							if (_T('A') <= source->key && source->key <= _T('Z'))
							{
								CString value;
								value.Format(_T("%c"), source-> key);
								result += value;
							}
							// F1～F24 文字列
							else if (VK_F1 <= source->key && source->key <= VK_F24)
							{
								CString value;
								value.Format(_T("F%d"), (int)(1 + source->key - VK_F1));
								result += value;
							}
							else
							{
								// その他アクセラレーターキー
								BOOL find = FALSE;
								size_t value = sizeof(::m_aValueKey) / sizeof(ASSOC);
								for (size_t index = 0; FALSE == find && index < value; index++)
								{
									LPASSOC value = &::m_aValueKey[index];
									find = source->key == value->key;
									if (find)
									{
										result += value->value;
									}
								};
							}
							// メニューバーへアクセラレータ文字列を設定する
							pMenuItemInfo->dwTypeData = (LPTSTR)(LPCTSTR)result;
							pMenu->SetMenuItemInfo(source->cmd, pMenuItemInfo, FALSE);
						}
					}// メニューアイテムに設定された文字列の取得失敗
				};
				// アクセラレータ情報取得配列の解放
				::free(pAccel);
				pAccel = nullptr;
			}
			// イメージリスト描画構造体を構築
			LPIMAGELISTDRAWPARAMS source = new IMAGELISTDRAWPARAMS;
			if (nullptr != source)
			{
				int value = sizeof(IMAGELISTDRAWPARAMS);
				::SecureZeroMemory(source, value);
				source->cbSize = value;
				// イメージリストを取得
				CImageList* pImagelist = m_wndToolBar.GetToolBarCtrl().GetImageList();
				if (nullptr != pImagelist)
				{
					source->himl = pImagelist->m_hImageList;
					// 画面互換ビットマップ作成用DCを取得
					CDC* pDC = GetDC();
					if (nullptr != pDC)
					{
						// 描画用DCを構築
						CDC dc;
						if (dc.CreateCompatibleDC(pDC))
						{
							source->hdcDst = dc.m_hDC;
							source->cx = 16;
							source->cy = 15;
							source->rgbBk = ::GetSysColor(COLOR_MENU);
							pMenuItemInfo->fMask = MIIM_BITMAP;
							result = TRUE;
							value = m_wndToolBar.GetCount();
							for (int index = 0; result && index < value; index++)
							{
								CString value;
								UINT nID = 0, nStyle = 0;
								// ボタンへ設定されているな用を取得
								m_wndToolBar.GetButtonInfo(index, nID, nStyle, source->i);
								switch (nID)
								{
								case ID_SEPARATOR:
									break;
								default:
									result = FALSE;
									// ボタンのテキストを取得
									if (value.LoadString(nID))
									{
										result = ::AfxExtractSubString(value, value, 2);
										if (!result)
										{
											result = ::AfxExtractSubString(value, value, 1);
										}
										if (result)
										{
											m_wndToolBar.SetButtonText(index, value);
										}
									}
									// ツールバーへ設定されているビットマップ情報を取得
									if (nullptr != source)
									{
										// ビットマップを作成
										pMenuItemInfo->hbmpItem = ::CreateCompatibleBitmap(pDC->m_hDC, source->cx, source->cy);
										if (nullptr != pMenuItemInfo->hbmpItem && INVALID_HANDLE_VALUE != pMenuItemInfo->hbmpItem)
										{
											// DC へビットマップを設定
											HBITMAP hOld = (HBITMAP)dc.SelectObject(pMenuItemInfo->hbmpItem);
											// イメージリストを使ってDCへ設定されたビットマップへボタンを描画する
											result = pImagelist->DrawIndirect(source);
											// DC からビットマップを取得
											pMenuItemInfo->hbmpItem = (HBITMAP)dc.SelectObject(hOld);
											if (result)
											{
												// 描画が成功したので、メニューバーへ設定する
												result = pMenu->SetMenuItemInfo(nID, pMenuItemInfo);
											}
										}
									}
									break;
								}
							};
							// 描画用DCを破棄
							dc.DeleteDC();
						}
						// 画面互換ビットマップ作成用DCを解放
						ReleaseDC(pDC);
						pDC = nullptr;
					}
				}
				// イメージリスト描画構造体を破棄
				delete source;
				source = nullptr;
			}
			// メニューアイテム情報設定構造体の破棄
			::free(pMenuItemInfo);
			pMenuItemInfo = nullptr;
		}// メニューアイテム情報作成失敗
	}
	if (result)
	{
		m_wndToolBar.SetSizes({ 31,37 }, { 16,15 });
	}
	// ステータスバーのメッセージ表示ペインの幅を最小に設定する
	int index = m_wndStatusBar.CommandToIndex(ID_SEPARATOR);
	switch (index)
	{
	case IDC_STATIC:
		break;
	default:
		if (TRUE)
		{
			UINT nID = 0, nStyle = 0;
			int cxWidth = 0;
			m_wndStatusBar.GetPaneInfo(index, nID, nStyle, cxWidth);
			cxWidth = 1;
			m_wndStatusBar.SetPaneInfo(index, nID, nStyle, cxWidth);
		}
		break;
	}
	// ウィンドウの起動サイズを設定
	SetWindowPos(nullptr, 0, 0, 838, 335, SWP_NOMOVE | SWP_NOZORDER);
	// アイドルメッセージを表示
	MessageBox(AFX_IDS_IDLEMESSAGE, MB_ICONINFORMATION);
	// インターバルタイマー開始
	SetTimer(ID_TIMER_INTERVAL, 128, nullptr);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return TRUE;
}

// CMainFrame の診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame メッセージ ハンドラー




void CMainFrame::OnUpdateFileOpen(CCmdUI* pCmdUI)
{
	if (nullptr != pCmdUI)
	{
		BOOL result = FALSE;
		CDocument* pDoc = GetActiveDocument();
		if (nullptr != pDoc)
		{
			result = pDoc->GetPathName().IsEmpty();
		}
		pCmdUI->Enable(result);
	}
}


void CMainFrame::OnUpdateFileNew(CCmdUI* pCmdUI)
{
	if (nullptr != pCmdUI)
	{
		BOOL result = FALSE;
		CDocument* pDoc = GetActiveDocument();
		if (nullptr != pDoc)
		{
			result = !pDoc->GetPathName().IsEmpty();
		}
		pCmdUI->Enable(result);
	}
}


void CMainFrame::OnUpdateIndicatorRow(CCmdUI* pCmdUI)
{
	if (nullptr != pCmdUI)
	{
		BOOL result = FALSE;
		int index = 0;
		CDocument* pDoc = GetActiveDocument();
		if (nullptr != pDoc)
		{
			result = !pDoc->GetPathName().IsEmpty();
			if (result)
			{
				CMFCApplication1View* pView =
					(CMFCApplication1View*)GetActiveView();
				if (nullptr != pView)
				{
					index = 1 + pView->GetRow();
				}
			}
		}
		CString value;
		value.Format(ID_INDICATOR_ROW, index);
		pCmdUI->SetText(value);
		pCmdUI->Enable(result);
		index = m_wndStatusBar.CommandToIndex(pCmdUI->m_nID);
		switch (index)
		{
		case IDC_STATIC:
			break;
		default:
			m_wndStatusBar.GetStatusBarCtrl().SendMessage(
				SB_SETTEXT, (WPARAM)SBT_OWNERDRAW | index, (LPARAM)result);
			break;
		}
	}
}


void CMainFrame::OnUpdateIndicatorCol(CCmdUI* pCmdUI)
{
	BOOL result = FALSE;
	int index = 0;
	CDocument* pDoc = GetActiveDocument();
	if (nullptr != pDoc)
	{
		result = !pDoc->GetPathName().IsEmpty();
		if (result)
		{
			CMFCApplication1View* pView =
				(CMFCApplication1View*)GetActiveView();
			if (nullptr != pView)
			{
				index = 1 + pView->GetCol();
			}
		}
	}
	CString value;
	value.Format(ID_INDICATOR_COL, index);
	pCmdUI->SetText(value);
	pCmdUI->Enable(result);
	index = m_wndStatusBar.CommandToIndex(pCmdUI->m_nID);
	switch (index)
	{
	case IDC_STATIC:
		break;
	default:
		m_wndStatusBar.GetStatusBarCtrl().SendMessage(
			SB_SETTEXT, (WPARAM)SBT_OWNERDRAW | index, (LPARAM)result);
		break;
	}
}


void CMainFrame::OnUpdateIndicatorModify(CCmdUI* pCmdUI)
{
	if (nullptr != pCmdUI)
	{
		BOOL result = FALSE;
		CDocument* pDoc = GetActiveDocument();
		if (nullptr != pDoc)
		{
			result = pDoc->IsModified();
		}
		pCmdUI->Enable(result);
		int index = m_wndStatusBar.CommandToIndex(pCmdUI->m_nID);
		switch (index)
		{
		case IDC_STATIC:
			break;
		default:
			m_wndStatusBar.GetStatusBarCtrl().SendMessage(
				SB_SETTEXT, (WPARAM)SBT_OWNERDRAW | index, (LPARAM)result);
			break;
		}
	}
}


void CMainFrame::OnUpdateIndicatorDate(CCmdUI* pCmdUI)
{
	if (nullptr != pCmdUI)
	{
		CString value;
		value.Format(_T("%04d/%02d/%02d"),
			m_systemTime.wYear,
			m_systemTime.wMonth,
			m_systemTime.wDay);
		pCmdUI->SetText(value);
		pCmdUI->Enable(TRUE);
	}
}


void CMainFrame::OnUpdateIndicatorTime(CCmdUI* pCmdUI)
{
	if (nullptr != pCmdUI)
	{
		CString value;
		value.Format(
			m_systemTime.wMilliseconds < 500 ?
			_T("%02d:%02d") : _T("%02d %02d"),
			m_systemTime.wHour, m_systemTime.wMinute);
		pCmdUI->SetText(value);
		pCmdUI->Enable(TRUE);
	}
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case ID_TIMER_INTERVAL:
		::GetLocalTime(&m_systemTime);
		break;
	}
	CFrameWnd::OnTimer(nIDEvent);
}


void CMainFrame::OnClose()
{
	BOOL result = TRUE;
	if (m_wndToolBar.IsWindowVisible())
	{
		CDocument* pDoc = GetActiveDocument();
		if (nullptr != pDoc)
		{
			if (pDoc->IsModified())
			{
				result = pDoc->SaveModified();
				if (result)
				{
					pDoc->SetModifiedFlag(FALSE);
				}
			}
			else
			{
				result = IDYES ==
					MessageBox(AFX_IDP_ASK_TO_EXIT,
						MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
			}
			if (result)
			{
				KillTimer(ID_TIMER_INTERVAL);
			}
		}
	}
	if (result)
	{
		CFrameWnd::OnClose();
	}
}


UINT CMainFrame::MessageBox(UINT nID, UINT style)
{
	BOOL result = IDOK;
	CString value;
	if (value.LoadString(nID))
	{
		result = MessageBox(value, style);
	}
	return result;
}


UINT CMainFrame::MessageBox(LPCTSTR caption, UINT style)
{
	UINT index = MB_ICONEXCLAMATION | MB_ICONINFORMATION;
	index &= style;
	index >>= 4;
	{
		HICON value = nullptr;
		int result = m_wndStatusBar.CommandToIndex(ID_SEPARATOR);
		switch (result)
		{
		case IDC_STATIC:
			break;
		default:
			if ((int)index < m_imgSmall.GetImageCount())
			{
				value = m_imgSmall.ExtractIcon(index);
			}
			if (nullptr == value)
			{
				value = m_imgSmall.ExtractIcon(4);
			}
			if (nullptr != value)
			{
				m_wndStatusBar.GetStatusBarCtrl().SetIcon(result, value);
			}
			break;
		}
	}
	CString value;
	if (value.LoadString(IDS_EDIT_TITLE))
	{
		::AfxExtractSubString(value, value, index);
	}
	SendMessage(WM_SETMESSAGESTRING, 0, (LPARAM)caption);
	BOOL result = IDOK;
	switch (style)
	{
	case MB_ICONINFORMATION:
		break;
	default:
		result = CWnd::MessageBox(caption, value, style);
		break;
	}
	switch (result)
	{
	case IDCANCEL:
	case IDNO:
		MessageBox(IDS_AFXBARRES_CANCEL, MB_ICONINFORMATION);
		break;
	case IDABORT:
		MessageBox(IDS_EDIT_ABORT, MB_ICONINFORMATION);
		break;
	}
	return result;
}


void CMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CString source = m_wndStatusBar.GetPaneText(lpDrawItemStruct->itemID);
	COLORREF value = ::GetSysColor(COLOR_GRAYTEXT);
	if (lpDrawItemStruct->itemData)
	{
		value = ::GetSysColor(COLOR_BTNTEXT);
	}
	::SetTextColor(lpDrawItemStruct->hDC, value);
	::SetBkColor(lpDrawItemStruct->hDC, ::GetSysColor(COLOR_MENU));
	::SetBkMode(lpDrawItemStruct->hDC, OPAQUE);
	::TextOut(
		lpDrawItemStruct->hDC,
		lpDrawItemStruct->rcItem.left,
		lpDrawItemStruct->rcItem.top,
		source, source.GetLength()
	);
}


void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);

	// アイコン
	HICON value = nullptr;
	// ステータスバーへアイコンの書き込み先があるかどうかを確認
	int index = m_wndStatusBar.CommandToIndex(ID_SEPARATOR);
	switch (index)
	{
	case IDC_STATIC:
		// 発見できなかった
		break;
	default:
		switch (nItemID)
		{
		case ID_SEPARATOR:
			// メニューバーのセパレータにはアイコンが設定できない
			break;
		default:
			if (TRUE)
			{
				// アイコン取得
				int result = m_wndToolBar.CommandToIndex(nItemID);
				// ツールバーにアイコンの取得先があるかどうかを確認
				switch (result)
				{
				case IDC_STATIC:
					// ボタンが発見できなかった
					break;
				default:
					if (TRUE)
					{
						// イメージリストを取得
						CImageList* source = m_wndToolBar.GetToolBarCtrl().GetImageList();
						if (nullptr != source)
						{
							UINT nID = 0, nStyle = 0;
							// ツールバーのインデックス長合からイメージ番号を取得
							m_wndToolBar.GetButtonInfo(result, nID, nStyle, result);
							// ツールバーのアイコンを取得
							value = source->ExtractIcon(result);
						}
					}
					break;
				}
			}
			break;
		}
		// アイコンが空の場合、情報アイコンを設定
		if (nullptr == value)
		{
			value = m_imgSmall.ExtractIcon(4);
		}
		// ツールチップのアイコンとタイトルを設定
		m_wndStatusBar.GetStatusBarCtrl().SetIcon(index, value);
		break;
	}
}


BOOL CMainFrame::OnTtnNeedtext(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(id);
	*pResult = 0;
	BOOL result = FALSE;

	if (nullptr != pNMHDR)
	{
		NMTTDISPINFO* source = (NMTTDISPINFO*)pNMHDR;
		UINT nID = (UINT)pNMHDR->idFrom;
		CString value;
		switch (TTF_IDISHWND & source->uFlags)
		{
		case TTF_IDISHWND:
			break;
		default:
			switch (nID)
			{
			case ID_SEPARATOR:
				// ツールバーのセパレータでは何もしない
				break;
			default:
				// ボタンの説明用文字列の取得
				if (value.LoadString(nID))
				{
					// ツールバーのツールチップのキャプション設定
					if (::AfxExtractSubString(m_sToolTip, value, 0))
					{
						source->lpszText = (LPTSTR)(LPCTSTR)m_sToolTip;
						result = TRUE;
					}
					// ツールバーのツールチップのタイトル取得
					if (::AfxExtractSubString(value, value, 1))
					{
						// メニューバーからショートカットキーの設定を取得
						CMenu* source = GetMenu();
						if (nullptr != source)
						{
							// メニューアイテムの文字列を取得
							CString result;
							source->GetMenuString(nID, result, MF_BYCOMMAND);
							// メニューアイテム文字列の後半のショートカット文字列を取得
							if (!result.IsEmpty() &&
								::AfxExtractSubString(result, result, 1, _T('\t')))
							{
								// ショートカット文字列が取れたらタイトル文字列へカッコ付きで追加する
								value += _T(" (");
								value += result;
								value += _T(")");
							}
						}
						// アイコン取得
						HICON result = nullptr;
						{
							// コマンドIDからツールバーのボタンの位置を取得
							int index = m_wndToolBar.CommandToIndex(nID);
							switch (index)
							{
							case IDC_STATIC:
								// ツールバーのコマンドボタンの位置が取れなかった
								break;
							default:
								if (TRUE)
								{
									// ツールバーからイメージリストを取得
									CImageList* source = m_wndToolBar.GetToolBarCtrl().GetImageList();
									if (nullptr != source)
									{
										UINT nStyle = 0;
										// ツールバーからイメージ番号を取得
										m_wndToolBar.GetButtonInfo(index, nID, nStyle, index);
										// アイコンを取得
										result = source->ExtractIcon(index);
									}
								}
								break;
							}
						}
						// アイコンが空の場合、情報アイコンを設定
						if (nullptr == result)
						{
							result = (HICON)TTI_INFO;
						}
						// ツールチップのアイコンとタイトルを設定
						::SendMessage(pNMHDR->hwndFrom, TTM_SETTITLE, (WPARAM)result, (LPARAM)(LPCTSTR)value);
					}
				}
				break;
			}
			break;
		}
	}
	return result;
}


void CMainFrame::OnUpdateIndicatorCaps(CCmdUI* pCmdUI)
{
	if (nullptr != pCmdUI)
	{
		BOOL result = FALSE;
		switch (pCmdUI->m_nID)
		{
		case ID_INDICATOR_CAPS:
			result = 0 != (1 & ::GetKeyState(VK_CAPITAL));
			break;
		case ID_INDICATOR_NUM:
			result = 0 != (1 & ::GetKeyState(VK_NUMLOCK));
			break;
		case ID_INDICATOR_SCRL:
			result = 0 != (1 & ::GetKeyState(VK_SCROLL));
			break;
		}
		pCmdUI->Enable(result);
		int index = m_wndStatusBar.CommandToIndex(pCmdUI->m_nID);
		switch (index)
		{
		case IDC_STATIC:
			break;
		default:
			m_wndStatusBar.GetStatusBarCtrl().SendMessage(
				SB_SETTEXT, (WPARAM)SBT_OWNERDRAW | index, (LPARAM)result);
			break;
		}
	}
}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 450;
	lpMMI->ptMinTrackSize.y = 164;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}
