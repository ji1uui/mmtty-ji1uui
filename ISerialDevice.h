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
// ISerialDevice.h
//
// SOLID: Dependency Inversion Principle (DIP) + Interface Segregation Principle (ISP)
//
// 問題: Main.h が CComm/CCtnc/CCradio の具体クラスに直接依存していた。
//       各デバイスが互換性のない独自APIを持っていた。
//
// 解決: シリアル通信デバイスの共通抽象インターフェースを定義。
//       CComm/CCtnc/CCradio が同一インターフェースを実装することで
//       上位モジュールが具体クラスに依存しなくなる。
//---------------------------------------------------------------------------
#ifndef ISerialDeviceH
#define ISerialDeviceH

#include <windows.h>

//---------------------------------------------------------------------------
// ISerialDevice
//
// CComm / CCtnc / CCradio の共通インターフェース。
// 使用側 (Main.h 等) はこのインターフェース経由でデバイスを操作する。
//---------------------------------------------------------------------------
class ISerialDevice {
public:
    virtual ~ISerialDevice() {}

    // デバイスが開いているか
    virtual BOOL IsOpen() const = 0;

    // デバイスを閉じる
    virtual void Close() = 0;

    // 非同期クローズ要求
    virtual void ReqClose() = 0;

    // クローズ完了まで待機
    virtual void WaitClose() = 0;

    // バイト列を送信
    virtual void Write(const void *pData, DWORD len) = 0;

    // 1文字送信 (CComm=BYTE, CCtnc/CCradio=char だが値域は同じ)
    virtual void PutChar(BYTE c) = 0;

    // 送信バッファが使用中か
    virtual int TxBusy() = 0;

    // PTT制御 (sw=1: 送信, sw=0: 受信)
    virtual void SetPTT(int sw) = 0;
};

#endif
