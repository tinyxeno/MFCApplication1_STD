
// MainFrm.h : CMainFrame クラスのインターフェイス
//

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // シリアル化からのみ作成します。
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:

// 操作
public:

// オーバーライド
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	/// <summary>
	/// メッセージボックスの処理
	/// </summary>
	/// <param name="nID">メッセージ文字列のID</param>
	/// <param name="style">メッセージボックスのスタイル</param>
	/// <returns>処理結果</returns>
	UINT MessageBox(UINT nID, UINT style = MB_ICONEXCLAMATION);
	/// <summary>
	/// メッセージボックスの処理
	/// </summary>
	/// <param name="nID">メッセージ文字列</param>
	/// <param name="style">メッセージボックスのスタイル</param>
	/// <returns>処理結果</returns>
	UINT MessageBox(LPCTSTR caption, UINT style = MB_ICONEXCLAMATION);
// 実装
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	/// <summary>
	/// 小さいアイコンのイメージリスト
	/// </summary>
	CImageList m_imgSmall;
	/// <summary>
	/// 大きいアイコンのイメージリスト
	/// </summary>
	CImageList m_imgLarge;

protected:  // コントロール バー用メンバー
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	/// <summary>
	/// 現在日時の保持先
	/// </summary>
	SYSTEMTIME        m_systemTime;
	CString m_sToolTip;

// 生成された、メッセージ割り当て関数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	/// <summary>
	/// ファイルを「開く」処理の許可禁止処理
	/// </summary>
	/// <param name="pCmdUI"></param>
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	/// <summary>
	/// 「新規作成」「ファイルを保存」「名前をつけて保存」などの許可/禁止処理
	/// </summary>
	/// <param name="pCmdUI"></param>
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	/// <summary>
	/// ステータスバーの行位置表示処理
	/// </summary>
	/// <param name="pCmdUI"></param>
	afx_msg void OnUpdateIndicatorRow(CCmdUI* pCmdUI);
	/// <summary>
	/// ステータスバーの列位置表示処理
	/// </summary>
	/// <param name="pCmdUI"></param>
	afx_msg void OnUpdateIndicatorCol(CCmdUI* pCmdUI);
	/// <summary>
	/// ステータスバーの編集中表示処理
	/// </summary>
	/// <param name="pCmdUI"></param>
	afx_msg void OnUpdateIndicatorModify(CCmdUI* pCmdUI);
	/// <summary>
	/// ステータスバーの日付表示処理
	/// </summary>
	/// <param name="pCmdUI"></param>
	afx_msg void OnUpdateIndicatorDate(CCmdUI* pCmdUI);
	/// <summary>
	/// ステータスバーの時刻表示処理
	/// </summary>
	/// <param name="pCmdUI"></param>
	afx_msg void OnUpdateIndicatorTime(CCmdUI* pCmdUI);
	/// <summary>
	/// インターバルタイマー処理
	/// </summary>
	/// <param name="nIDEvent"></param>
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	/// <summary>
	/// 終了処理
	/// </summary>
	afx_msg void OnClose();
	/// <summary>
	/// ステータスバーのペインの処理
	/// </summary>
	/// <param name="nIDCtl"></param>
	/// <param name="lpDrawItemStruct">オーナー描画構造体</param>
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	/// <summary>
	/// メニューアイテム選択時の処理
	/// </summary>
	/// <param name="nItemID"></param>
	/// <param name="nFlags"></param>
	/// <param name="hSysMenu"></param>
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	/// <summary>
	/// ツールバーのツールチップ表示処理
	/// </summary>
	/// <param name="id"></param>
	/// <param name="pNMHDR"></param>
	/// <param name="pResult"></param>
	/// <returns></returns>
	afx_msg BOOL OnTtnNeedtext(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	/// <summary>
	/// 「CAPS」「NUM」「SCRL」の表示処理
	/// </summary>
	/// <param name="pCmdUI"></param>
	afx_msg void OnUpdateIndicatorCaps(CCmdUI* pCmdUI);
	/// <summary>
	/// メインフレームの最小の大きさの処理
	/// </summary>
	/// <param name="lpMMI"></param>
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};


