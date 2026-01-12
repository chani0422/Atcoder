#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd -- "$SCRIPT_DIR/.." && pwd)"
cd "$ROOT_DIR"

BACKUP_DIR=".add_markers_backup_v2_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$BACKUP_DIR"

added=0
skipped=0

add_one() {
  local f="$1"

  # 変換済み形式のみ
  grep -qE '^[[:space:]]*#ifdef[[:space:]]+INCLUDED_MAIN' "$f" || return 1
  grep -qE '^[[:space:]]*#else' "$f" || return 1
  grep -qE '^[[:space:]]*#endif' "$f" || return 1

  # すでに入ってたらスキップ
  grep -q 'LIB_BEGIN' "$f" && return 1

  mkdir -p "$BACKUP_DIR/$(dirname "$f")"
  cp -p "$f" "$BACKUP_DIR/$f"

  tmp="$(mktemp)"

  awk '
    {
      if($0 ~ /^[[:space:]]*#else/){
        print
        print "// === LIB_BEGIN ==="
        next
      }
      if($0 ~ /^[[:space:]]*#endif/){
        print "// === LIB_END ==="
        print
        next
      }
      print
    }
  ' "$f" > "$tmp"

  mv "$tmp" "$f"
  return 0
}

for d in ABC[0-9][0-9][0-9]; do
  for p in A B C D E F G; do
    f="$d/$p.cpp"
    [[ -f "$f" ]] || continue
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
echo "backup  : $BACKUP_DIR/"
