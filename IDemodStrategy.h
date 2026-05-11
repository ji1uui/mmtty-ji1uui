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
// IDemodStrategy.h
//
// SOLID: Open/Closed Principle (OCP) + Single Responsibility Principle (SRP)
//
// 問題: CFSKDEM クラスが m_type (VERFFTDEM=1, VERAA6YQ=2) という int フラグで
//       if-else 分岐により復調アルゴリズムを切り替えていた。
//       新しい復調方式を追加するたびに CFSKDEM を修正する必要があった。
//
// 解決: Strategy パターンにより復調アルゴリズムを抽象化。
//       - IDemodStrategy: 復調戦略の共通インターフェース
//       - CPhaseDemodStrategy: CPHASE を使う位相法 (VERFFTDEM=1)
//       - CAA6YQDemodStrategy: CAA6YQ を使う相関法 (VERAA6YQ=2)
//       CFSKDEM はインターフェース経由で復調を行い、具体実装に依存しない。
//       新しい復調方式は IDemodStrategy を実装するだけで追加できる (OCP)。
//---------------------------------------------------------------------------
#ifndef IDemodStrategyH
#define IDemodStrategyH

//---------------------------------------------------------------------------
// IDemodStrategy
//
// FSK 復調アルゴリズムの抽象インターフェース。
// CFSKDEM はこのインターフェース経由で復調処理を委譲する。
//---------------------------------------------------------------------------
class IDemodStrategy {
public:
    virtual ~IDemodStrategy() {}

    // サンプリング周波数を設定する
    virtual void SetSampleFreq(double sampleFreq) = 0;

    // マーク周波数を設定する
    virtual void SetMarkFreq(double markFreq) = 0;

    // スペース周波数を設定する
    virtual void SetSpaceFreq(double spaceFreq) = 0;

    // 現在のマーク周波数を返す
    virtual double GetMarkFreq() const = 0;

    // 現在のスペース周波数を返す
    virtual double GetSpaceFreq() const = 0;

    // 1サンプルを処理する
    virtual void Do(double sample) = 0;

    // マーク出力レベルを返す
    virtual double GetMarkOut() const = 0;

    // スペース出力レベルを返す
    virtual double GetSpaceOut() const = 0;

    // 内部状態をリセットする
    virtual void Reset() = 0;
};

//---------------------------------------------------------------------------
// CPhaseDemodStrategy
//
// CPHASE を用いた位相法 FSK 復調 (旧 VERFFTDEM=1 相当)。
// CFSKDEM::m_type == VERFFTDEM の処理をこのクラスに移譲する。
//---------------------------------------------------------------------------
class CPHASE;   // 前方宣言

class CPhaseDemodStrategy : public IDemodStrategy {
private:
    CPHASE *m_pPhase;
    double  m_markOut;
    double  m_spaceOut;
    double  m_markFreq;
    double  m_spaceFreq;

public:
    CPhaseDemodStrategy(CPHASE *pPhase)
        : m_pPhase(pPhase)
        , m_markOut(0.0)
        , m_spaceOut(0.0)
        , m_markFreq(2125.0)
        , m_spaceFreq(2295.0)
    {}

    void SetSampleFreq(double sampleFreq);
    void SetMarkFreq(double markFreq);
    void SetSpaceFreq(double spaceFreq);

    double GetMarkFreq()  const { return m_markFreq; }
    double GetSpaceFreq() const { return m_spaceFreq; }

    void Do(double sample);

    double GetMarkOut()  const { return m_markOut; }
    double GetSpaceOut() const { return m_spaceOut; }

    void Reset();
};

//---------------------------------------------------------------------------
// CAA6YQDemodStrategy
//
// CAA6YQ を用いた相関法 FSK 復調 (旧 VERAA6YQ=2 相当)。
// CFSKDEM::m_type == VERAA6YQ の処理をこのクラスに移譲する。
//---------------------------------------------------------------------------
class CAA6YQ;   // 前方宣言

class CAA6YQDemodStrategy : public IDemodStrategy {
private:
    CAA6YQ *m_pAA6YQ;
    double  m_markOut;
    double  m_spaceOut;
    double  m_markFreq;
    double  m_spaceFreq;

public:
    CAA6YQDemodStrategy(CAA6YQ *pAA6YQ)
        : m_pAA6YQ(pAA6YQ)
        , m_markOut(0.0)
        , m_spaceOut(0.0)
        , m_markFreq(2125.0)
        , m_spaceFreq(2295.0)
    {}

    void SetSampleFreq(double sampleFreq);
    void SetMarkFreq(double markFreq);
    void SetSpaceFreq(double spaceFreq);

    double GetMarkFreq()  const { return m_markFreq; }
    double GetSpaceFreq() const { return m_spaceFreq; }

    void Do(double sample);

    double GetMarkOut()  const { return m_markOut; }
    double GetSpaceOut() const { return m_spaceOut; }

    void Reset();
};

//---------------------------------------------------------------------------
// DemodStrategyFactory
//
// Factory パターン: m_type 値から適切な IDemodStrategy を生成する。
// CFSKDEM 内の new CPHASE / new CAA6YQ を一か所に集約することで
// 生成ロジックの変更が CFSKDEM に波及しない (OCP)。
//
// 使い方:
//   IDemodStrategy *s = DemodStrategyFactory::Create(type, &m_Phase, &m_AA6YQ);
//---------------------------------------------------------------------------
class DemodStrategyFactory {
public:
    // type: VERFFTDEM=1 → CPhaseDemodStrategy
    //       VERAA6YQ=2  → CAA6YQDemodStrategy
    static IDemodStrategy* Create(int type, CPHASE *pPhase, CAA6YQ *pAA6YQ);
};

#endif
