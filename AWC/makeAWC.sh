#!/usr/bin/env bash
set -euo pipefail

# 使い方: ./makeAWC.sh 1  （AWC001 を作成）
n="${1:-}"
if [[ ! "$n" =~ ^[0-9]+$ ]]; then
  echo "Usage: $0 1  (creates AWC001)" >&2
  exit 1
fi

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

# LIB_PATH を指定した場合はそれを使う。指定がなければ既存の ABC 用 lib.cpp を探す。
if [[ -z "${LIB_PATH:-}" ]]; then
  for candidate in \
    "$SCRIPT_DIR/lib.cpp" \
    "$SCRIPT_DIR/../ABC/lib.cpp" \
    "$SCRIPT_DIR/../ABC/tools/lib.cpp"; do
    if [[ -f "$candidate" ]]; then
      LIB_PATH="$candidate"
      break
    fi
  done
fi

if [[ -z "${LIB_PATH:-}" || ! -f "$LIB_PATH" ]]; then
  echo "ERROR: lib.cpp not found. Set LIB_PATH to its path." >&2
  exit 1
fi

printf -v number '%03d' "$((10#$n))"
dir="$SCRIPT_DIR/AWC$number"
mkdir -p "$dir"

for prob in A B C D E; do
  file="$dir/$prob.cpp"
  if [[ ! -s "$file" ]]; then
    echo "init template: $file"
    {
      cat <<EOF
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
//${prob}問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);

    return 0;
}
//解答コードここまで

#else

EOF
      cat "$LIB_PATH"
      printf '\n#endif\n'
    } > "$file"
  fi
done

echo "Created: $dir/{A..E}.cpp with lib from $LIB_PATH"
