//Copyright+LGPL

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 2000-2013 Makoto Mori, Nobuyuki Oba, Dave Bernstein
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of MMTTY.

// MMTTY is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// MMTTY is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License along with MMTTY.  If not, see 
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------



///----------------------------------------------------------
///  Radio�R�~�j���P�[�V�����N���X
///
///  (C) JE3HHT Makoto.Mori
///
//---------------------------------------------------------------------------
#ifndef CradioH
#define CradioH
#include "ComLib.h"
#include "mmlink.h"
#include "ISerialDevice.h"
#include "IRadioProtocol.h"
//---------------------------------------------------------------------------
#include <Classes.hpp>
typedef struct {
	int		change;

	char	StrPort[32];	// �|�[�g�̖��O
	int		BaudRate;		// �{�[���[�g
	int		BitLen;			// 0-7Bit, 1-8Bit
	int		Stop;			// 0-1Bit, 1-2Bit
	int		Parity;			// 0-PN, 1-PE, 2-PO
	int		flwXON;			// Xon/Xoff ON
	int		flwCTS;			// CTS-RTS ON
	int		usePTT;

	long	ByteWait;		// �o�C�g�Ԃ̑��M�E�G�C�g

	int		Cmdxx;

	AnsiString	CmdInit;
	AnsiString	CmdRx;
	AnsiString	CmdTx;

	AnsiString	cmdGNR;
	int		openGNR;

	int		PollType;
	int		PollInterval;
	int		PollOffset;

	int		PollScan;
}CRADIOPARA;
extern	CRADIOPARA	RADIO;
//---------------------------------------------------------------------------
void LoadRADIOSetup(TMemIniFile *pIniFile);
void SaveRADIOSetup(TMemIniFile *pIniFile);
//#define	CR	0x0d
//#define	LF	0x0a
#define	CRADIO_CLOSE	1
#define	RADIO_COMBUFSIZE	4096
#define	RADIO_TXBUFSIZE		256
#define	RADIO_RXBUFSIZE		256

enum {
	RADIO_POLLNULL,
	RADIO_POLLYAESUHF,
	RADIO_POLLYAESUVU,
	RADIO_POLLICOM,
	RADIO_POLLICOMN,
	RADIO_POLLOMNIVI,
	RADIO_POLLOMNIVIN,
	RADIO_POLLKENWOOD,
	RADIO_POLLKENWOODN,
	RADIO_POLLFT1000D,
	RADIO_POLLFT920,
	RADIO_POLLJST245,
	RADIO_POLLJST245N,
    RADIO_POLLFT9000,   //1.66B AA6YQ add new radios at end of list as this value is stored in mmtty.ini
    RADIO_POLLFT2000,   //1.66B AA6YQ
    RADIO_POLLFT950,    //1.66B AA6YQ
	RADIO_POLLFT450,    //1.66B AA6YQ
	RADIO_POLLFT991,    //1.70E AA6YQ
	RADIO_POLLFT891,    //1.70E AA6YQ
};

//---------------------------------------------------------------------------
// SOLID: DIP + ISP + OCP
//
// CCradio は ISerialDevice を実装することで Main.h が具体クラスに依存しなくなる。
// IRadioProtocol を内包することで無線機種別プロトコルの切り替えが
// CCradio 本体を変更せずに行えるようになる (OCP)。
// m_pProtocol に RadioProtocolFactory::Create() で生成したオブジェクトを
// セットするだけで新機種に対応できる。
//---------------------------------------------------------------------------
class CCradio : public TThread, public ISerialDevice
{
public:
	BOOL	m_CreateON;		// �N���G�C�g�t���O
	DCB		m_dcb;			// �c�b�a
	HANDLE	m_fHnd;			// �t�@�C���n���h��
	HWND	m_wHnd;			// �e�̃E�C���h�E�n���h��
	UINT	m_uMsg;
	UINT	m_ID;			// ���b�Z�[�W�̂h�c�ԍ�
	volatile	int	m_Command;		// �X���b�h�ւ̃R�}���h
	BOOL	m_TxAbort;		// ���M���~�t���O
	AnsiString	Name;
	CMMRadio	*m_pRadio;

	int		m_PSKGNRId;
	int 	m_OpenGNR;

	char	m_txbuf[RADIO_TXBUFSIZE];
	int		m_txcnt;
	int		m_txwp;
	int		m_txrp;

	int		m_PollCnt;
	unsigned char m_rxbuf[RADIO_RXBUFSIZE];
	int		m_rxcnt;
	int		m_FreqEvent;
	char	m_Freq[32];
	int		m_MarkFreq;

	int		m_ScanAddr;

	// IRadioProtocol Strategy: 無線機種別プロトコルを注入可能にする (OCP)
	// RadioProtocolFactory::Create(RADIO.PollType) で生成したオブジェクトをセットする
	IRadioProtocol  *m_pProtocol;
protected:
	void virtual __fastcall Execute();
	BOOL 	OpenPipe(CRADIOPARA *cp, HWND hwnd, UINT nID);
	void CatchPoll(BYTE c);
	void FreqYaesuHF(void);
	void FreqYaesuVU(void);
	void FreqICOM(void);
	void FreqKenwood(void);
    void FreqYaesu9K2K(void);
	void FreqJST245(void);

public:
	__fastcall CCradio(bool CreateSuspended);
	__fastcall ~CCradio(){
		Close();
		delete m_pProtocol;
	};

	// --- ISerialDevice 実装 ---
	BOOL    IsOpen()  const  { return m_CreateON; }
	void    Close(void);
	void    ReqClose(void);
	void    WaitClose(void);
	void    Write(const void *pData, DWORD len);    // ISerialDevice::Write
	void    PutChar(BYTE c);                         // ISerialDevice::PutChar
	int     TxBusy(void);
	void    SetPTT(int sw);
	// --- ISerialDevice 実装 終わり ---

	inline void UpdateHandle(HWND hwnd, UINT uMsg){
		m_wHnd = hwnd; m_uMsg = uMsg;
	};

	// プロトコルオブジェクトを差し替える (OCP: 新機種追加時に CCradio を変更不要)
	void SetProtocol(IRadioProtocol *pProtocol) {
		delete m_pProtocol;
		m_pProtocol = pProtocol;
	}
	IRadioProtocol* GetProtocol() const { return m_pProtocol; }

	BOOL Open(CRADIOPARA *cp, HWND hwnd, UINT uMsg, UINT nID);
	DWORD RecvLen(void);
	DWORD Read(BYTE *p, DWORD len);
	void OutStr(LPCSTR fmt, ...);
	void OutLine(LPCSTR fmt, ...);
	void SendCommand(LPCSTR p);
	void Timer(int tx, int interval);

	inline LPCSTR GetFreq(void){
		m_FreqEvent = 0;
		return m_Freq;
	};
	inline void SetMarkFreq(int f){
		m_MarkFreq = f;
	};
	int IsFreqChange(LPCSTR p);
	void UpdateFreq(double freq);
};

void InitRADIOPara(void);
#endif

