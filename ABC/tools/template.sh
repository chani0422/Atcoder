#!/usr/bin/env bash
set -euo pipefail

TEMPLATE='
#ifdef INCLUDED_MAIN

//解答コードここから
//__PROBLEM__問題
__BODY__
 //解答コードここまで

#else


// === LIB_BEGIN ===
#include <bits/stdc++.h>
using namespace std;

#define rep(i,a,b) for(int i=(a); i<(b); i++)
#define rrep(i,a,b) for(int i=(a); i>=(b); i--)
#define pb(x) push_back(x)
#define popb() pop_back()
#define fi first
#define se second
#define yn(x) do { if(x) cout << "Yes\n"; else cout << "No\n"; } while(0)
#define nall(a) a.begin(),a.end()
#define rall(a) a.rbegin(),a.rend()
#define For(x) for(auto a : (x)) cout << a << " ";

using ll = long long;
using ld = long double;
using ull = unsigned long long;
using Graph = vector<vector<ll>>;

template<class A, class B>
ostream& operator<<(ostream& os, const pair<A,B>& p) {
    return os << "(" << p.first << "," << p.second << ")";
}

template<class T>
void For2(const T& a) {
    for (const auto& row : a) {
        for (const auto& v : row) cout << v << " ";
        cout << "\n";
    }
}
// === LIB_END ===

#define SELF_FILE "__PROBLEM__.cpp"
#define INCLUDED_MAIN
#include SELF_FILE

#endif
'

BACKUP_DIR=".convert_backup_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$BACKUP_DIR"

changed=0
skipped=0
missing=0

# 既存コードから「先頭テンプレ（include/define/using/typedef/空行/コメント）」を剥がして、
# それ以降（グローバル変数/関数/dfs/bfs/main 等）を全部残す
extract_body() {
  local file="$1"
  awk '
    BEGIN{start=0}
    {
      if(start==0){
        line=$0
        if(line ~ /^[[:space:]]*$/) next
        if(line ~ /^[[:space:]]*\/\/.*/) next
        if(line ~ /^[[:space:]]*#include[[:space:]]*[<"].*[>"].*$/) next
        if(line ~ /^[[:space:]]*using[[:space:]]+namespace[[:space:]]+std[[:space:]]*;[[:space:]]*$/) next
        if(line ~ /^[[:space:]]*#define[[:space:]]+/) next
        if(line ~ /^[[:space:]]*typedef[[:space:]].*;[[:space:]]*$/) next
        # using は 1行に複数書いてても落とすため「; を含む using 行は全部捨てる」
        if(line ~ /^[[:space:]]*using[[:space:]].*;[[:space:]]*$/) next
        start=1
      }
      print
    }
  ' "$file"
}

for d in $(find . -maxdepth 1 -type d -name 'ABC[0-9][0-9][0-9]' | sed 's|^\./||' | sort); do
  for prob in A B C D E F G; do
    f="$d/$prob.cpp"
    if [[ ! -f "$f" ]]; then
      ((++missing))
      continue
    fi

    # すでにテンプレ形式ならスキップ
    if grep -qE '^\s*#ifdef\s+INCLUDED_MAIN\b' "$f"; then
      ((++skipped))
      continue
    fi

    body="$(extract_body "$f")"
    if [[ -z "${body//[[:space:]]/}" ]]; then
      ((++skipped))
      continue
    fi

    # バックアップ
    mkdir -p "$BACKUP_DIR/$d"
    cp -p "$f" "$BACKUP_DIR/$f"

    tmp="$(mktemp)"

    # __BODY__ を安全に埋め込む（sedで & や / を壊さない）
    # 1) bodyをファイルへ
    bodytmp="$(mktemp)"
    printf "%s\n" "$body" > "$bodytmp"

    # 2) TEMPLATEを展開して、__PROBLEM__ 置換
    # 3) __BODY__ を bodytmp の内容に差し替え
    awk -v prob="$prob" -v bodyfile="$bodytmp" '
      BEGIN{
        # body読み込み
        while((getline line < bodyfile) > 0){
          body = body line "\n"
        }
        close(bodyfile)
      }
      {
        gsub(/__PROBLEM__/, prob)
        if($0 ~ /__BODY__/){
          sub(/__BODY__/, "")
          printf "%s", body
        } else {
          print
        }
      }
    ' <(printf "%s\n" "$TEMPLATE") > "$tmp"

    rm -f "$bodytmp"
    mv "$tmp" "$f"
    ((++changed))
  done
done

echo "Done."
echo "changed : $changed"
echo "skipped : $skipped"
echo "missing : $missing"
echo "backup  : $BACKUP_DIR/"
