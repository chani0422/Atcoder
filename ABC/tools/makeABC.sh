//最新版
#!/usr/bin/env bash
set -euo pipefail

# 使い方: ./makeABC.sh 437
n="${1:-}"
if [[ -z "$n" ]]; then
  echo "Usage: $0 437"
  exit 1
fi

# makeABC.sh と同じ場所にある lib.cpp を読む（違う場所なら LIB_PATH を変える）
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
LIB_PATH="${LIB_PATH:-$SCRIPT_DIR/lib.cpp}"

if [[ ! -f "$LIB_PATH" ]]; then
  echo "ERROR: lib.cpp not found: $LIB_PATH"
  exit 1
fi

dir="ABC$n"
mkdir -p "$dir"
touch "$dir"/{A,B,C,D,E,F,G}.cpp

# 空ファイルにだけテンプレを入れる
for file in "$dir"/*.cpp; do
  if [[ ! -s "$file" ]]; then
    prob="$(basename "$file" .cpp)"  # A.cpp -> A
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

      # ここで lib.cpp を挿し込む
      cat "$LIB_PATH"

      cat <<EOF

#endif
EOF
    } > "$file"
  fi
done

echo "Created: $dir/{A..G}.cpp with lib from $LIB_PATH"
