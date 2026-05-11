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
// IRadioProtocol.h
//
// SOLID: Open/Closed Principle (OCP)
//
// 問題: CCradio クラスが RADIO_POLLYAESUHF, RADIO_POLLICOM, RADIO_POLLKENWOOD 等
//       の enum と巨大な switch 文で各ラジオ機種のプロトコルを処理していた。
//       新しいラジオ機種を追加するたびに enum を拡張し、CCradio を修正し、
//       全体を再コンパイルする必要があった (OCP 違反)。
//
// 解決: Strategy パターンにより各ラジオ機種のプロトコル処理を分離。
//       - IRadioProtocol: プロトコル処理の共通インターフェース
//       - CYaesuHFProtocol: YAESU HF 機のプロトコル
//       - CIcomProtocol:    ICOM 機のプロトコル
//       - CKenwoodProtocol: Kenwood 機のプロトコル
//       新機種を追加するには IRadioProtocol を実装するだけでよい (OCP)。
//       CCradio は IRadioProtocol 経由でプロトコル処理を委譲する (DIP)。
//
// デザインパターン: Strategy (GoF)
//---------------------------------------------------------------------------
#ifndef IRadioProtocolH
#define IRadioProtocolH

#include <windows.h>

//---------------------------------------------------------------------------
// IRadioProtocol
//
// 無線機制御プロトコルの抽象インターフェース。
// CCradio はこのインターフェース経由でプロトコル処理を委譲する。
//---------------------------------------------------------------------------
class IRadioProtocol {
public:
    virtual ~IRadioProtocol() {}

    // ポーリングコマンド文字列を返す (NULL 終端)
    virtual const char* GetPollCommand() const = 0;

    // 推奨ポーリング間隔 (ミリ秒) を返す
    virtual int GetPollIntervalMs() const = 0;

    // 受信バッファから周波数文字列 (Hz) を解析して freqOut に書き込む
    // 戻り値: 解析成功=true, 未完了=false
    virtual bool ParseFrequency(const unsigned char *buf, int len, char *freqOut, int freqOutSize) const = 0;

    // 周波数 (Hz) を設定するコマンド文字列を cmdOut に書き込む
    virtual void FormatSetFrequencyCmd(double freqHz, char *cmdOut, int cmdOutSize) const = 0;

    // PTT オンコマンドを cmdOut に書き込む
    virtual void FormatPTTOnCmd(char *cmdOut, int cmdOutSize) const = 0;

    // PTT オフコマンドを cmdOut に書き込む
    virtual void FormatPTTOffCmd(char *cmdOut, int cmdOutSize) const = 0;

    // このプロトコルのラジオ種別名 (ログ等に使用)
    virtual const char* GetProtocolName() const = 0;
};

//---------------------------------------------------------------------------
// CYaesuHFProtocol
//
// YAESU HF 機 (FT-1000D, FT-950, FT-2000 等) の CAT プロトコル。
// 旧 CCradio::FreqYaesuHF() / RADIO_POLLYAESUHF の処理を分離。
//---------------------------------------------------------------------------
class CYaesuHFProtocol : public IRadioProtocol {
public:
    const char* GetPollCommand() const  { return "IF;"; }
    int GetPollIntervalMs() const       { return 500; }

    bool ParseFrequency(const unsigned char *buf, int len, char *freqOut, int freqOutSize) const;
    void FormatSetFrequencyCmd(double freqHz, char *cmdOut, int cmdOutSize) const;
    void FormatPTTOnCmd(char *cmdOut, int cmdOutSize) const;
    void FormatPTTOffCmd(char *cmdOut, int cmdOutSize) const;
    const char* GetProtocolName() const { return "YAESU HF"; }
};

//---------------------------------------------------------------------------
// CYaesuVUProtocol
//
// YAESU V/U 機 (FT-817, FT-857 等) の CAT プロトコル。
// 旧 CCradio::FreqYaesuVU() / RADIO_POLLYAESUVU の処理を分離。
//---------------------------------------------------------------------------
class CYaesuVUProtocol : public IRadioProtocol {
public:
    const char* GetPollCommand() const  { return "\x00\x00\x00\x00\x03"; }
    int GetPollIntervalMs() const       { return 500; }

    bool ParseFrequency(const unsigned char *buf, int len, char *freqOut, int freqOutSize) const;
    void FormatSetFrequencyCmd(double freqHz, char *cmdOut, int cmdOutSize) const;
    void FormatPTTOnCmd(char *cmdOut, int cmdOutSize) const;
    void FormatPTTOffCmd(char *cmdOut, int cmdOutSize) const;
    const char* GetProtocolName() const { return "YAESU V/U"; }
};

//---------------------------------------------------------------------------
// CIcomProtocol
//
// ICOM 機の CI-V プロトコル。
// 旧 CCradio::FreqICOM() / RADIO_POLLICOM の処理を分離。
//---------------------------------------------------------------------------
class CIcomProtocol : public IRadioProtocol {
private:
    unsigned char m_civAddr;    // CI-V アドレス (機種依存)
public:
    CIcomProtocol(unsigned char civAddr = 0x58) : m_civAddr(civAddr) {}

    const char* GetPollCommand() const  { return NULL; }    // CI-V は別形式
    int GetPollIntervalMs() const       { return 500; }

    bool ParseFrequency(const unsigned char *buf, int len, char *freqOut, int freqOutSize) const;
    void FormatSetFrequencyCmd(double freqHz, char *cmdOut, int cmdOutSize) const;
    void FormatPTTOnCmd(char *cmdOut, int cmdOutSize) const;
    void FormatPTTOffCmd(char *cmdOut, int cmdOutSize) const;
    const char* GetProtocolName() const { return "ICOM CI-V"; }
};

//---------------------------------------------------------------------------
// CKenwoodProtocol
//
// Kenwood 機の CAT プロトコル。
// 旧 CCradio::FreqKenwood() / RADIO_POLLKENWOOD の処理を分離。
//---------------------------------------------------------------------------
class CKenwoodProtocol : public IRadioProtocol {
public:
    const char* GetPollCommand() const  { return "IF;"; }
    int GetPollIntervalMs() const       { return 500; }

    bool ParseFrequency(const unsigned char *buf, int len, char *freqOut, int freqOutSize) const;
    void FormatSetFrequencyCmd(double freqHz, char *cmdOut, int cmdOutSize) const;
    void FormatPTTOnCmd(char *cmdOut, int cmdOutSize) const;
    void FormatPTTOffCmd(char *cmdOut, int cmdOutSize) const;
    const char* GetProtocolName() const { return "Kenwood"; }
};

//---------------------------------------------------------------------------
// CJst245Protocol
//
// Japan Radio JST-245 のプロトコル。
// 旧 CCradio::FreqJST245() / RADIO_POLLJST245 の処理を分離。
//---------------------------------------------------------------------------
class CJst245Protocol : public IRadioProtocol {
public:
    const char* GetPollCommand() const  { return "\\"; }
    int GetPollIntervalMs() const       { return 500; }

    bool ParseFrequency(const unsigned char *buf, int len, char *freqOut, int freqOutSize) const;
    void FormatSetFrequencyCmd(double freqHz, char *cmdOut, int cmdOutSize) const;
    void FormatPTTOnCmd(char *cmdOut, int cmdOutSize) const;
    void FormatPTTOffCmd(char *cmdOut, int cmdOutSize) const;
    const char* GetProtocolName() const { return "JST-245"; }
};

//---------------------------------------------------------------------------
// RadioProtocolFactory
//
// Factory パターン: RADIO_POLL* 定数から IRadioProtocol を生成する。
//
// CCradio 内の具体プロトコルオブジェクトの生成をここに集約することで、
// プロトコル追加時に CCradio 本体を変更しなくてよくなる (OCP)。
//
// 使い方:
//   IRadioProtocol *p = RadioProtocolFactory::Create(RADIO.PollType);
//---------------------------------------------------------------------------
class RadioProtocolFactory {
public:
    // pollType: RADIO_POLL* 定数に対応するプロトコルオブジェクトを生成して返す
    // 戻り値: 新規生成した IRadioProtocol (呼び出し元が delete する責任を持つ)
    //         未知の pollType の場合は NULL を返す
    static IRadioProtocol* Create(int pollType);
};

#endif
