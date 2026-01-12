#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd -- "$SCRIPT_DIR/.." && pwd)"
cd "$ROOT_DIR"

BACKUP_DIR="$ROOT_DIR/.add_markers_backup_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$BACKUP_DIR"

added=0
skipped=0
missing=0

add_one() {
  local f="$1"

  # 変換済み形式だけ
  grep -qE '^[[:space:]]*#ifdef[[:space:]]+INCLUDED_MAIN([[:space:]]|$)' "$f" || return 2

  # すでに入ってたらスキップ
  if grep -q 'LIB_BEGIN' "$f" || grep -q 'LIB_END' "$f"; then
    return 2
  fi

  # #else と #define SELF_FILE が無いなら触らない
  grep -qE '^[[:space:]]*#else([[:space:]]|$)' "$f" || return 2
  grep -qE '^[[:space:]]*#define[[:space:]]+SELF_FILE([[:space:]]|$)' "$f" || return 2

  mkdir -p "$BACKUP_DIR/$(dirname "$f")"
  cp -p "$f" "$BACKUP_DIR/$f"

  local tmp
  tmp="$(mktemp)"

  awk '
    BEGIN{state=0}
    {
      # #else を見つけたら、次に出力する非空行の直前で LIB_BEGIN を入れる
      if($0 ~ /^[[:space:]]*#else([[:space:]]|$)/){
        print
        state=1
        next
      }

      # #define SELF_FILE の直前で LIB_END を入れる
      if(state>0 && $0 ~ /^[[:space:]]*#define[[:space:]]+SELF_FILE([[:space:]]|$)/){
        print "// === LIB_END ==="
        state=0
        print
        next
      }

      # #else の後、最初の非空行の前に LIB_BEGIN
      if(state==1 && $0 !~ /^[[:space:]]*$/){
        print "// === LIB_BEGIN ==="
        state=2
      }

      print
    }
  ' "$f" > "$tmp"

  mv "$tmp" "$f"
  return 0
}

for d in $(find . -maxdepth 1 -type d -name 'ABC[0-9][0-9][0-9]' | sed 's|^\./||' | sort); do
  for prob in A B C D E F G; do
    f="$d/$prob.cpp"
    if [[ ! -f "$f" ]]; then
      ((++missing))
      continue
    fi
    if add_one "$f"; then
      ((++added))
    else
      ((++skipped))
    fi
  done
done

echo "Done."
echo "added   : $added"
echo "skipped : $skipped"
echo "missing : $missing"
echo "backup  : $BACKUP_DIR/"
