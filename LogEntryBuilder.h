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
// LogEntryBuilder.h
//
// SOLID: Single Responsibility Principle (SRP)
//
// 問題: SDMMLOG 構造体のフィールド設定が複数箇所に散在していた。
//       ログエントリの構築ロジックが Main.cpp, QsoDlg.cpp 等に重複していた。
//       必須フィールドの漏れチェックや初期化がなかった。
//
// 解決: Builder パターンにより SDMMLOG の構築を一か所に集約。
//       - メソッドチェーンで可読性の高いエントリ構築
//       - 各 Set* メソッドが対応フィールドの設定のみを担当 (SRP)
//       - Build() 呼び出しで完成した SDMMLOG を返す
//
// デザインパターン: Builder (GoF)
//---------------------------------------------------------------------------
#ifndef LogEntryBuilderH
#define LogEntryBuilderH

#include "LogFile.h"
#include <string.h>

//---------------------------------------------------------------------------
// LogEntryBuilder
//
// SDMMLOG (ログエントリ構造体) を段階的に構築するビルダークラス。
//
// 使い方:
//   SDMMLOG entry = LogEntryBuilder()
//       .SetDateTime(24, 0x1505, 1330, 1345)
//       .SetCallSign("JA1ABC")
//       .SetRST("599", "589")
//       .SetBandFreq(BAND_20M, 50)
//       .SetMode(MODE_RTTY)
//       .SetName("TARO")
//       .Build();
//---------------------------------------------------------------------------
class LogEntryBuilder {
private:
    SDMMLOG m_entry;

public:
    LogEntryBuilder() {
        memset(&m_entry, 0, sizeof(m_entry));
    }

    // 既存エントリをベースにしたビルダーを作成 (コピー編集用)
    explicit LogEntryBuilder(const SDMMLOG &base) {
        m_entry = base;
    }

    // 日時フィールドを設定する
    // year: 年 (下2桁, 例: 2024年→24)
    // date: 日付 (上位バイト=月, 下位バイト=日)
    // btime: 開始時刻 (HHMM)
    // etime: 終了時刻 (HHMM)
    LogEntryBuilder& SetDateTime(BYTE year, WORD date, WORD btime, WORD etime) {
        m_entry.year  = year;
        m_entry.date  = date;
        m_entry.btime = btime;
        m_entry.etime = etime;
        return *this;
    }

    // コールサインを設定する (最大 MLCALL 文字)
    LogEntryBuilder& SetCallSign(const char *call) {
        strncpy(m_entry.call, call, MLCALL);
        m_entry.call[MLCALL] = '\0';
        return *this;
    }

    // 送受信 RST レポートを設定する
    // ur: 相手局 RST (最大 MLRST 文字)
    // my: 自局 RST (最大 MLRST 文字)
    LogEntryBuilder& SetRST(const char *ur, const char *my) {
        strncpy(m_entry.ur, ur, MLRST);
        m_entry.ur[MLRST] = '\0';
        strncpy(m_entry.my, my, MLRST);
        m_entry.my[MLRST] = '\0';
        return *this;
    }

    // バンドと周波数オフセットを設定する
    // band: バンドコード (CLogFile が定義するバンド定数)
    // fq: 周波数オフセット (kHz 単位の差分)
    LogEntryBuilder& SetBandFreq(BYTE band, SHORT fq) {
        m_entry.band = band;
        m_entry.fq   = fq;
        return *this;
    }

    // 通信モードを設定する
    LogEntryBuilder& SetMode(BYTE mode) {
        m_entry.mode = mode;
        return *this;
    }

    // 送信電力を設定する (最大 MLPOW 文字)
    LogEntryBuilder& SetPower(const char *pow) {
        strncpy(m_entry.pow, pow, MLPOW);
        m_entry.pow[MLPOW] = '\0';
        return *this;
    }

    // オペレーター名を設定する (最大 MLNAME 文字)
    LogEntryBuilder& SetName(const char *name) {
        strncpy(m_entry.name, name, MLNAME);
        m_entry.name[MLNAME] = '\0';
        return *this;
    }

    // QTH (所在地) を設定する (最大 MLQTH 文字)
    LogEntryBuilder& SetQTH(const char *qth) {
        strncpy(m_entry.qth, qth, MLQTH);
        m_entry.qth[MLQTH] = '\0';
        return *this;
    }

    // QSL 情報を設定する (最大 MLQSL 文字)
    LogEntryBuilder& SetQSL(const char *qsl, char send = 0, char recv = 0) {
        strncpy(m_entry.qsl, qsl, MLQSL);
        m_entry.qsl[MLQSL] = '\0';
        m_entry.send = send;
        m_entry.recv = recv;
        return *this;
    }

    // 備考 (リマーク) を設定する (最大 MLREM 文字)
    LogEntryBuilder& SetRemarks(const char *rem) {
        strncpy(m_entry.rem, rem, MLREM);
        m_entry.rem[MLREM] = '\0';
        return *this;
    }

    // CQ/Call フラグを設定する
    LogEntryBuilder& SetCQ(char cq) {
        m_entry.cq = cq;
        return *this;
    }

    // 環境フィールドを設定する
    LogEntryBuilder& SetEnv(WORD env) {
        m_entry.env = env;
        return *this;
    }

    // オプションフィールド1を設定する (最大 MLOPT 文字)
    LogEntryBuilder& SetOpt1(const char *opt) {
        strncpy(m_entry.opt1, opt, MLOPT);
        m_entry.opt1[MLOPT] = '\0';
        return *this;
    }

    // オプションフィールド2を設定する (最大 MLOPT 文字)
    LogEntryBuilder& SetOpt2(const char *opt) {
        strncpy(m_entry.opt2, opt, MLOPT);
        m_entry.opt2[MLOPT] = '\0';
        return *this;
    }

    // 構築した SDMMLOG を返す
    SDMMLOG Build() const {
        return m_entry;
    }

    // 構築した SDMMLOG を既存のポインタにコピーする
    void BuildInto(SDMMLOG *sp) const {
        if (sp) *sp = m_entry;
    }
};

#endif
