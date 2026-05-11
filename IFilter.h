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
// IFilter.h
//
// SOLID: Interface Segregation Principle (ISP) + Liskov Substitution Principle (LSP)
//
// 問題: CFIR/CFIR2/CFIRX/CIIR/CLMS 等が互換性のない独自APIを持ち、
//       ポリモーフィックに扱えなかった (条件分岐でタイプ判定が必要)。
//
// 解決: すべてのスカラーフィルタが実装すべき最小インターフェースを定義。
//       フィルタをポリモーフィックに扱えるようになり、Compositeや
//       動的なフィルタチェーン構成が可能になる。
//---------------------------------------------------------------------------
#ifndef IFilterH
#define IFilterH

//---------------------------------------------------------------------------
// IFilter
//
// スカラー入出力フィルタ (CFIR/CIIR/CLMS/CAVG 等) の共通インターフェース。
// Process(double) → double の変換を保証する。
//---------------------------------------------------------------------------
class IFilter {
public:
    virtual ~IFilter() {}

    // 1サンプルを処理して出力を返す
    virtual double Process(double input) = 0;

    // 内部状態をリセットする
    virtual void Clear() = 0;
};

//---------------------------------------------------------------------------
// CompositeFilter
//
// Composite パターン: 複数の IFilter を直列に結合したフィルタチェーン。
//
// SOLID: Open/Closed Principle (OCP)
// 問題: TSound でフィルタ処理の順序がハードコーディングされていた。
// 解決: フィルタチェーンを動的に構築・変更できる。
//---------------------------------------------------------------------------
class CompositeFilter : public IFilter {
private:
    IFilter   **m_filters;
    int         m_count;
    int         m_capacity;

public:
    CompositeFilter() : m_filters(NULL), m_count(0), m_capacity(0) {}

    ~CompositeFilter() {
        delete[] m_filters;
    }

    // フィルタを末尾に追加する (所有権は呼び出し元が保持)
    void Add(IFilter *filter) {
        if (m_count >= m_capacity) {
            int newCap = m_capacity == 0 ? 4 : m_capacity * 2;
            IFilter **newBuf = new IFilter*[newCap];
            for (int i = 0; i < m_count; i++) newBuf[i] = m_filters[i];
            delete[] m_filters;
            m_filters = newBuf;
            m_capacity = newCap;
        }
        m_filters[m_count++] = filter;
    }

    double Process(double input) {
        double out = input;
        for (int i = 0; i < m_count; i++) {
            out = m_filters[i]->Process(out);
        }
        return out;
    }

    void Clear() {
        for (int i = 0; i < m_count; i++) {
            m_filters[i]->Clear();
        }
    }

    int GetCount() const { return m_count; }
};

#endif
