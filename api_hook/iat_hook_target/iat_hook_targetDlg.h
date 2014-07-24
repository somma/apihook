
// iat_hook_sampleDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// Ciat_hook_sampleDlg 대화 상자
class Ciat_hook_sampleDlg : public CDialogEx
{
// 생성입니다.
public:
	Ciat_hook_sampleDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_IAT_HOOK_SAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_editbox;
	afx_msg void OnBnClickedButton1();
};
