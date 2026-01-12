#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd -- "$SCRIPT_DIR/.." && pwd)"
cd "$ROOT_DIR"

LIB_FILE="$ROOT_DIR/tools/lib.cpp"
BACKUP_DIR="$ROOT_DIR/.update_lib_backup_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$BACKUP_DIR"

updated=0
skipped=0
missing=0

[[ -f "$LIB_FILE" ]] || { echo "ERROR: $LIB_FILE が見つかりません"; exit 1; }

# lib.cpp からはマーカー行を除外して使う（入ってても安全）
LIB_CLEAN="$(mktemp)"
awk '
  $0 ~ /=== LIB_BEGIN ===/ {next}
  $0 ~ /=== LIB_END ===/   {next}
  {print}
' "$LIB_FILE" > "$LIB_CLEAN"

update_one() {
  local f="$1"

  # マーカーが無いなら更新しない
  grep -q '=== LIB_BEGIN ===' "$f" || return 2
  grep -q '=== LIB_END ===' "$f"   || return 2

  mkdir -p "$BACKUP_DIR/$(dirname "$f")"
  cp -p "$f" "$BACKUP_DIR/$f"

  local tmp
  tmp="$(mktemp)"

  awk -v libfile="$LIB_CLEAN" '
    BEGIN{
      # libfile を丸ごと読み込む（文字列にしないので壊れない）
      lib=""
      while((getline line < libfile) > 0){
        lib = lib line "\n"
      }
      close(libfile)
      inside=0
      inserted=0
    }
    {
      if($0 ~ /=== LIB_BEGIN ===/){
        print
        if(inserted==0){
          printf "%s", lib
          inserted=1
        }
        inside=1
        next
      }
      if($0 ~ /=== LIB_END ===/){
        inside=0
        inserted=0
        print
        next
      }
      if(inside==0) print
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

    if update_one "$f"; then
      ((++updated))
    else
      ((++skipped))
    fi
  done
done

rm -f "$LIB_CLEAN"

echo "Done."
echo "updated : $updated"
echo "skipped : $skipped"
echo "missing : $missing"
echo "backup  : $BACKUP_DIR/"
