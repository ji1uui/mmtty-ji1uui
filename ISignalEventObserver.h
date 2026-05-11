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
// ISignalEventObserver.h
//
// SOLID: Dependency Inversion Principle (DIP)
//
// 問題: TSound スレッドが TMmttyWd (UI フォーム) に直接 Synchronize コールで
//       アクセスしていた。下位モジュール (音声処理) が上位モジュール (UI) に
//       依存するという DIP 違反だった。
//
// 解決: Observer パターンにより音声処理層と UI 層を疎結合にする。
//       - ISignalEventObserver: イベント購読インターフェース
//       - TSound は ISignalEventObserver のリストにイベントを通知する
//       - TMmttyWd は ISignalEventObserver を実装して TSound に登録する
//       これにより TSound は UI 層の具体クラスを知らなくてよくなる (DIP)。
//
// デザインパターン: Observer (GoF)
//---------------------------------------------------------------------------
#ifndef ISignalEventObserverH
#define ISignalEventObserverH

#include <windows.h>

//---------------------------------------------------------------------------
// ISignalEventObserver
//
// TSound からの信号処理イベントを受け取るオブザーバーインターフェース。
// TMmttyWd などの UI クラスがこれを実装して TSound に登録する。
//---------------------------------------------------------------------------
class ISignalEventObserver {
public:
    virtual ~ISignalEventObserver() {}

    // 受信キャラクタが確定した (ch: ASCII コード)
    virtual void OnRxCharReceived(int ch) = 0;

    // FFT データが更新された (pFFT: スペクトルデータ, size: 要素数)
    virtual void OnFFTUpdated(const int *pFFT, int size, int sampFreq) = 0;

    // スケルチ状態が変化した (sqOpen=true: 開放)
    virtual void OnSQChanged(bool sqOpen) = 0;

    // AFC により周波数が変化した
    virtual void OnFreqChanged(double markFreq, double spaceFreq) = 0;

    // XY スコープデータが更新された (pXY: [X0,Y0,X1,Y1,...])
    virtual void OnXYUpdated(const double *pXY) = 0;
};

//---------------------------------------------------------------------------
// SignalEventNotifier
//
// ISignalEventObserver のリストを管理し、各イベントを全登録オブザーバーに
// ブロードキャストするユーティリティクラス。
//
// TSound はこのクラスを内包して通知処理を委譲することで、
// リスト管理ロジックを Signal 処理コアから分離できる (SRP)。
//---------------------------------------------------------------------------
#define SIGNAL_OBSERVER_MAX 8

class SignalEventNotifier {
private:
    ISignalEventObserver *m_observers[SIGNAL_OBSERVER_MAX];
    int                   m_count;

public:
    SignalEventNotifier() : m_count(0) {
        for (int i = 0; i < SIGNAL_OBSERVER_MAX; i++) {
            m_observers[i] = NULL;
        }
    }

    // オブザーバーを登録する (重複登録は無視)
    bool Subscribe(ISignalEventObserver *observer) {
        for (int i = 0; i < m_count; i++) {
            if (m_observers[i] == observer) return false;
        }
        if (m_count >= SIGNAL_OBSERVER_MAX) return false;
        m_observers[m_count++] = observer;
        return true;
    }

    // オブザーバーの登録を解除する
    bool Unsubscribe(ISignalEventObserver *observer) {
        for (int i = 0; i < m_count; i++) {
            if (m_observers[i] == observer) {
                for (int j = i; j < m_count - 1; j++) {
                    m_observers[j] = m_observers[j + 1];
                }
                m_observers[--m_count] = NULL;
                return true;
            }
        }
        return false;
    }

    void NotifyRxChar(int ch) {
        for (int i = 0; i < m_count; i++) m_observers[i]->OnRxCharReceived(ch);
    }

    void NotifyFFT(const int *pFFT, int size, int sampFreq) {
        for (int i = 0; i < m_count; i++) m_observers[i]->OnFFTUpdated(pFFT, size, sampFreq);
    }

    void NotifySQ(bool sqOpen) {
        for (int i = 0; i < m_count; i++) m_observers[i]->OnSQChanged(sqOpen);
    }

    void NotifyFreq(double markFreq, double spaceFreq) {
        for (int i = 0; i < m_count; i++) m_observers[i]->OnFreqChanged(markFreq, spaceFreq);
    }

    void NotifyXY(const double *pXY) {
        for (int i = 0; i < m_count; i++) m_observers[i]->OnXYUpdated(pXY);
    }

    int GetObserverCount() const { return m_count; }
};

#endif
