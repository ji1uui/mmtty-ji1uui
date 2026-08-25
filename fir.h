//Copyright+LGPL

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 2000-2013 Makoto Mori, Nobuyuki Oba
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of MMTTY.

// MMTTY is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// MMTTY is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License along with MMTTY.  If not, see 
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------
#ifndef firH
#define firH
#include <inifiles.hpp>

//---------------------------------------------------------------------------
#include "ComLib.h"
#include "IFilter.h"
#define	TAPMAX	512

#define	PI	3.1415926535897932384626433832795

enum {
	ffLPF,
	ffHPF,
	ffBPF,
	ffBEF,
	ffUSER,
	ffLMS,
};

typedef struct {
	int		n;
	int		typ;
	double	fs;
	double	fcl;
	double	fch;
	double	att;
	double	gain;
	double	fc;
	double	hp[TAPMAX+1];		/* �W���z��		*/
}FIR;
void MakeFilter(double *HP, int tap, int type, double fs, double fcl, double fch, double att, double gain);
void MakeFilter(double *HP, FIR *fp);
void __fastcall MakeHilbert(double *H, int N, double fs, double fc1, double fc2);
void DrawGraph(Graphics::TBitmap *pBitmap, const double *H, int Tap, int Over, int &nmax, int init, TColor col);
void DrawGraphIIR(Graphics::TBitmap *pBitmap, double a0, double a1, double a2, double b1, double b2, int Over, int &nmax, int init, TColor col);
void DrawGraph2(Graphics::TBitmap *pBitmap, const double *H1, int Tap1, const double *H2, int Tap2, int Over, int &nmax, int init, TColor col);

double __fastcall DoFIR(double *hp, double *zp, double d, int tap);

class CDECM2{			// 36 Tap FIR 1/2 DECM
private:
	alignas(32) double	Z1[19];
	alignas(32) double	Z2[18];
	alignas(32) double	H[37];
public:
	CDECM2();
	double Do(double d1, double d2);
};

#if 0
class CDECM2H{			// 64 Tap FIR 1/2 DECM
private:
	double	Z1[33];
	double	Z2[32];
	double	H[65];
public:
	CDECM2H();
	double Do(double d1, double d2);
};
#endif

#if 0
class CINTP2{			// 36 Tap FIR x2 INTP
private:
	double	Z[19];
	double	H[37];
public:
	CINTP2();
	void Do(double &d1, double &d2, double d);
};

class CDECM3{			// 48 Tap FIR 1/3 DECM
private:
	double	Z1[17];
	double	Z2[16];
	double	Z3[16];
	double	H[49];
public:
	CDECM3();
	double Do(double d1, double d2, double d3);
};

class CINTP3{			// 48 Tap FIR x3 INTP
private:
	double	Z[17];
	double	H[49];
public:
	CINTP3();
	void Do(double &d1, double &d2, double &d3, double d);
};
#endif

#define	DELAYMAX	4096
// SOLID ISP/LSP: IFilter 実装によりフィルタチェーンで利用可能
class CLMS : public IFilter
{
private:
	double	*Z;					// FIR Z Application
	double	*D;					// LMS Delay;

	double	m_MarkFreq;			// for RTTY
	double	m_SpaceFreq;		// for RTTY
	double	HBPF[TAPMAX+1];		// for RTTY

	double	m_lmsADJSC;			// �X�P�[�������l
	double	m_lmsErr;			// LMS �덷�f�[�^
	double	m_lmsMErr;			// LMS �덷�f�[�^�i�~�Q�ʁj

	void GetFW(double &fl, double &fh, double fq);
public:
	int		m_Type;
	int		m_Tap;
	int		m_NotchTap;			// Notch Taps
	int		m_lmsInv;			// LMS InvOutput
	int		m_lmsDelay;			// LMS Delay
	int		m_lmsAGC;			// LMS AGC
	double	m_lmsMU2;			// LMS 2��
	double	m_lmsGM;			// LMS ��
	double	*H;					// �A�v���P�[�V�����t�B���^�̌W��

	int		m_lmsNotch;			// �m�b�`���g��
	int		m_lmsNotch2;		// �m�b�`���g��
	int		m_twoNotch;			// �Q�߂̃m�b�`�̋���
	int		m_bpf;				// For RTTY
public:
	CLMS();
	~CLMS();

	void Copy(CLMS &other);
	double Do(double d);
	void SetWindow(double mfq, double sfq);
	inline double *GetHBPF(void){return HBPF;};

	// IFilter 実装
	double Process(double input) { return Do(input); }
	void   Clear();
};

// SOLID ISP/LSP: IFilter を実装することでフィルタをポリモーフィックに扱える。
// CompositeFilter や動的フィルタチェーンで利用可能になる。
class CIIRTANK : public IFilter
{
private:
	double	z1, z2;
public:
	double	a0;
	double	b1, b2;
public:
	CIIRTANK();
	void SetFreq(double f, double smp, double bw);
	double Do(double d);

	// IFilter 実装
	double Process(double input) { return Do(input); }
	void   Clear()               { z1 = z2 = 0.0; }
};

#define	IIRMAX	16
// SOLID ISP/LSP: IFilter 実装により CompositeFilter で組み合わせ可能
class CIIR : public IFilter
{
private:
	double	*Z;
public:
	double	*A;
	double	*B;
	int		m_order;
	int		m_bc;
	double	m_rp;
	CIIR();
	~CIIR();
	void MakeIIR(double fc, double fs, int order, int bc, double rp);
	double Do(double d);
	void Clear(void);

	// IFilter 実装
	double Process(double input) { return Do(input); }
};

class CINTPXY2{			// IIR x2 INTP for XY Scope
private:
	CIIR	iir;
public:
	CINTPXY2();
	inline void Clear(void){iir.Clear();};
	void __fastcall Do(double *p, double d);
};

#if 0
class CINTPXY4{			// IIR x4 INTP for XY Scope
private:
	CIIR	iir;
public:
	CINTPXY4();
	inline void Clear(void){iir.Clear();};
	void __fastcall Do(double *p, double d);
};

class CINTPXY8{			// IIR x8 INTP for XY Scope
private:
	CIIR	iir;
public:
	CINTPXY8();
	inline void Clear(void){iir.Clear();};
	void __fastcall Do(double *p, double d);
};

class CINTPXY2FIR{			// 32 Tap FIR x2 INTP
private:
	double	Z[17];
	double	H[33];
public:
	CINTPXY2FIR();
	inline void Clear(void){memset(Z, 0, sizeof(Z));};
	void __fastcall Do(double *p, double d);
};
#endif

class CINTPXY4FIR{			// 64 Tap FIR x4 INTP
private:
	alignas(32) double	Z[17];
	alignas(32) double	H[65];
public:
	CINTPXY4FIR();
	inline void Clear(void){memset(Z, 0, sizeof(Z));};
	void __fastcall Do(double *p, double d);
};

class CINTPXY8FIR{			// 96 Tap FIR x8 INTP
private:
	alignas(32) double	Z[13];
	alignas(32) double	H[97];
public:
	CINTPXY8FIR();
	inline void Clear(void){memset(Z, 0, sizeof(Z));};
	void __fastcall Do(double *p, double d);
};

#define	OVERFIR	1
#if OVERFIR
class CDECM4{				// 80 Tap FIR 1/4 DECM
private:
	alignas(32) double	Z1[21];
	alignas(32) double	Z2[20];
	alignas(32) double	Z3[20];
	alignas(32) double	Z4[20];
	alignas(32) double	H[81];
public:
	CDECM4();
	double __fastcall Do(double *dp);
};

class CINTP4{			// 80 Tap FIR x4 INTP
private:
	alignas(32) double	Z[21];
	alignas(32) double	H[81];
public:
	CINTP4();
	inline void Clear(void){memset(Z, 0, sizeof(Z));};
	void __fastcall Do(double *p, double d);
};
#endif

class COVERLIMIT{
private:
#if OVERFIR
	CDECM4	decm;
	CINTP4	intp;
#else
	CIIR	iira;
	CIIR	iirb;
#endif
public:
	COVERLIMIT();
	double Do(double d, double lmt);
};

class CDECM2IIR{			// IIR 1/2 DECM
private:
	CIIR	iir;
public:
	CDECM2IIR();
	double Do(double d1, double d2);
};

#if 0
#define HILLTAP 48
class CHILL
{
public:
	double  Z[HILLTAP+1];
	double  H[HILLTAP+1];
	double  m_A[4];
	double  m_OFF;
	double  m_OUT;
	double  *m_ph;
	int     m_htap;
	int     m_tap;
	double	m_SampFreq;
	double	m_CenterFreq;
	double	m_MarkFreq;
	double	m_SpaceFreq;
public:
	CHILL();
	void SetSampFreq(double samp);
	void SetFreq(double mark, double space);
	void SetMarkFreq(double mark);
	void SetSpaceFreq(double space);

	double Do(double d);
};
#endif

void DrawGraphIIR(Graphics::TBitmap *pBitmap, CIIR *ip, int Over, int &nmax, int init, TColor col);
#endif

