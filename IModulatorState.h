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
// IModulatorState.h
//
// SOLID: Single Responsibility Principle (SRP)
//
// 問題: CFSKMOD クラスが送受信状態管理・信号生成・バッファ管理・
//       ディドル処理・タイマー管理をすべて混在して持っていた。
//       m_mode という int フラグに基づく if-else 分岐で状態を切り替えていた。
//
// 解決: State パターンにより変調器の状態遷移を明示的に分離。
//       - IModulatorState: 状態インターフェース
//       - CModIdleState:    アイドル状態 (送信バッファ空、ディドル送信中)
//       - CModTransmitState: 文字送信状態 (データ送信中)
//       - CModWaitState:    ウェイト状態 (タイマー待機中)
//       各状態クラスは自身の責任のみを持つ (SRP)。
//       CFSKMOD は状態オブジェクトに処理を委譲し、状態遷移を管理する。
//---------------------------------------------------------------------------
#ifndef IModulatorStateH
#define IModulatorStateH

class CFSKMOD;  // 前方宣言

//---------------------------------------------------------------------------
// IModulatorState
//
// CFSKMOD の送受信状態インターフェース。
// 各具体状態クラスがこのインターフェースを実装する。
//---------------------------------------------------------------------------
class IModulatorState {
public:
    virtual ~IModulatorState() {}

    // 状態に入った時の初期化処理
    virtual void Enter(CFSKMOD &mod) = 0;

    // 1サンプル分の処理を行い出力サンプルを返す
    virtual double Process(CFSKMOD &mod) = 0;

    // 状態から出る時のクリーンアップ処理
    virtual void Exit(CFSKMOD &mod) = 0;

    // この状態のアイドルフラグ (1=アイドル中)
    virtual int IsIdle() const = 0;
};

//---------------------------------------------------------------------------
// CModIdleState
//
// アイドル状態: 送信バッファが空でディドル信号を送出している状態。
// 旧コード: m_mode == 0 相当
//---------------------------------------------------------------------------
class CModIdleState : public IModulatorState {
public:
    CModIdleState() {}

    void Enter(CFSKMOD &mod);
    double Process(CFSKMOD &mod);
    void Exit(CFSKMOD &mod);
    int IsIdle() const { return 1; }
};

//---------------------------------------------------------------------------
// CModTransmitState
//
// データ送信状態: 送信バッファからキャラクタを取り出して変調する状態。
// 旧コード: m_mode == 1 相当
//---------------------------------------------------------------------------
class CModTransmitState : public IModulatorState {
public:
    CModTransmitState() {}

    void Enter(CFSKMOD &mod);
    double Process(CFSKMOD &mod);
    void Exit(CFSKMOD &mod);
    int IsIdle() const { return 0; }
};

//---------------------------------------------------------------------------
// CModWaitState
//
// ウェイト状態: キャラクタ間ウェイトタイマーを待機している状態。
// 旧コード: m_mode == 2 相当
//---------------------------------------------------------------------------
class CModWaitState : public IModulatorState {
public:
    CModWaitState() {}

    void Enter(CFSKMOD &mod);
    double Process(CFSKMOD &mod);
    void Exit(CFSKMOD &mod);
    int IsIdle() const { return 0; }
};

#endif
