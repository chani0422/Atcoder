#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd -- "$SCRIPT_DIR/.." && pwd)"
cd "$ROOT_DIR"

BACKUP_DIR="$ROOT_DIR/.migrate_template_backup_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$BACKUP_DIR"

converted=0
cleaned=0
already=0
skipped=0
missing=0

backup_file() {
  local f="$1"
  mkdir -p "$BACKUP_DIR/$(dirname "$f")"
  cp -p -- "$f" "$BACKUP_DIR/$f"
}

# 旧→新へ変換（旧テンプレがある場合）
convert_old_to_new() {
  local f="$1"

  # 旧テンプレ判定
  grep -qE '^[[:space:]]*#ifdef[[:space:]]+INCLUDED_MAIN([[:space:]]|$)' "$f" || return 2

  backup_file "$f"

  local tmp
  tmp="$(mktemp)"

  awk '
    BEGIN{ state=0 }
    {
      # 旧: #ifdef INCLUDED_MAIN を見つけたら、新ヘッダを出して以降を main 部分として出力
      if(state==0 && $0 ~ /^[[:space:]]*#ifdef[[:space:]]+INCLUDED_MAIN([[:space:]]|$)/){
        print "#if !__INCLUDE_LEVEL__"
        print "#include __FILE__"
        print ""
        state=1
        next
      }

      # main 部分：#else までをそのまま出力
      if(state==1){
        if($0 ~ /^[[:space:]]*#else([[:space:]]|$)/){
          print $0
          state=2
          next
        }
        print $0
        next
      }

      # lib 部分：旧テンプレ由来の再帰 include 行を消す
      if(state==2){
        if($0 ~ /^[[:space:]]*#define[[:space:]]+INCLUDED_MAIN([[:space:]]|$)/) next
        if($0 ~ /^[[:space:]]*#include[[:space:]]+__FILE__([[:space:]]|$)/) next
        print $0
        next
      }

      # それ以外（通常は来ないが保険）
      print $0
    }
  ' "$f" > "$tmp"

  # 権限を保つために上書きは cat で
  cat "$tmp" > "$f"
  rm -f "$tmp"

  # 最低限の検証
  grep -qE '^[[:space:]]*#if[[:space:]]*!__INCLUDE_LEVEL__' "$f" \
    || { echo "ERROR: convert failed: $f"; return 1; }

  return 0
}

# すでに新テンプレでも、旧の残骸があれば除去（安全側）
cleanup_residual_lines() {
  local f="$1"

  # 新テンプレ判定（この場合のみ掃除する）
  grep -qE '^[[:space:]]*#if[[:space:]]*!__INCLUDE_LEVEL__' "$f" || return 2

  # 消す対象が無ければ何もしない
  if ! grep -qE '^[[:space:]]*#define[[:space:]]+INCLUDED_MAIN([[:space:]]|$)|^[[:space:]]*#include[[:space:]]+__FILE__([[:space:]]|$)' "$f"; then
    return 1
  fi

  backup_file "$f"

  local tmp
  tmp="$(mktemp)"

  awk '
    {
      if($0 ~ /^[[:space:]]*#define[[:space:]]+INCLUDED_MAIN([[:space:]]|$)/) next
      if($0 ~ /^[[:space:]]*#include[[:space:]]+__FILE__([[:space:]]|$)/) next
      print $0
    }
  ' "$f" > "$tmp"

  cat "$tmp" > "$f"
  rm -f "$tmp"
  return 0
}

# 対象: ROOT直下の ABC### ディレクトリ / A～G.cpp
for d in $(find . -maxdepth 1 -type d -name 'ABC[0-9][0-9][0-9]' | sed 's|^\./||' | sort); do
  for prob in A B C D E F G; do
    f="$d/$prob.cpp"
    if [[ ! -f "$f" ]]; then
      ((++missing))
      continue
    fi

    if convert_old_to_new "$f"; then
      ((++converted))
      continue
    fi

    # 旧テンプレじゃない場合：新テンプレの掃除だけ試す
    if cleanup_residual_lines "$f"; then
      ((++cleaned))
      continue
    fi

    # 新テンプレだが掃除不要
    if grep -qE '^[[:space:]]*#if[[:space:]]*!__INCLUDE_LEVEL__' "$f"; then
      ((++already))
    else
      ((++skipped))
    fi
  done
done

echo "Done."
echo "converted (old->new) : $converted"
echo "cleaned  (residual)  : $cleaned"
echo "already (new)        : $already"
echo "skipped (unknown)    : $skipped"
echo "missing              : $missing"
echo "backup               : $BACKUP_DIR/"
