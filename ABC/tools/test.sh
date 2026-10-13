#!/usr/bin/env bash

set -u
set -o pipefail

ACL_DIR="/home/chanini/projects/kyopro/ac-library"
TIME_LIMIT="5s"

# ==========================================
# モード判定
#
# 通常:
#   test.sh C
#
# デバッグ:
#   test.sh --debug C
# ==========================================
MODE="normal"

if [[ "${1:-}" == "--debug" ]]; then
    MODE="debug"
    shift
fi

# ==========================================
# コンテスト名を現在地から取得
# 例: ABC468
# ==========================================
CONTEST="$(basename "$PWD")"

if [[ ! "$CONTEST" =~ ^ABC[0-9]+$ ]]; then
    echo "エラー: ABCxxxフォルダ内で実行してください"
    echo "現在地: $PWD"
    exit 1
fi

# ==========================================
# 問題を決定
#
# t c / td c:
#   C.cpp
#
# t / td:
#   最後に保存されたA.cpp～G.cpp
# ==========================================
PROBLEM="${1:-}"

if [[ -z "$PROBLEM" ]]; then
    SRC="$(ls -t [A-G].cpp 2>/dev/null | head -n 1 || true)"

    if [[ -z "$SRC" ]]; then
        echo "エラー: A.cpp～G.cppが見つかりません"
        exit 1
    fi

    PROBLEM="${SRC%.cpp}"
else
    PROBLEM="${PROBLEM%.cpp}"
    PROBLEM="${PROBLEM^^}"
    SRC="${PROBLEM}.cpp"
fi

if [[ ! "$PROBLEM" =~ ^[A-G]$ ]]; then
    echo "エラー: 問題名はA～Gで指定してください"
    echo "例: t c"
    echo "例: td c"
    exit 1
fi

if [[ ! -f "$SRC" ]]; then
    echo "エラー: $SRC が見つかりません"
    exit 1
fi

# ==========================================
# 保存場所
# ==========================================
BUILD_DIR=".build"
SAMPLE_DIR=".samples/$PROBLEM"

mkdir -p "$BUILD_DIR"

if [[ "$MODE" == "debug" ]]; then
    EXE="$BUILD_DIR/${PROBLEM}_debug"
    MODE_NAME="DEBUG"
else
    EXE="$BUILD_DIR/$PROBLEM"
    MODE_NAME="NORMAL"
fi

CONTEST_LOWER="${CONTEST,,}"
PROBLEM_LOWER="${PROBLEM,,}"

URL="https://atcoder.jp/contests/${CONTEST_LOWER}/tasks/${CONTEST_LOWER}_${PROBLEM_LOWER}"

echo
echo "======================================"
echo "  Contest : $CONTEST"
echo "  Problem : $PROBLEM"
echo "  Source  : $SRC"
echo "  Mode    : $MODE_NAME"
echo "======================================"
echo

# ==========================================
# コンパイルオプション
# ==========================================
CXXFLAGS=(
    -std=gnu++23
    -Wall
    -Wextra
    -fdiagnostics-color=always
    -I "$ACL_DIR"
)

if [[ "$MODE" == "debug" ]]; then
    CXXFLAGS+=(
        -O0
        -g3
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -D_GLIBCXX_DEBUG
    )
else
    CXXFLAGS+=(
        -O2
    )
fi

echo "[Compile] $SRC"

if ! /usr/bin/g++ \
    "${CXXFLAGS[@]}" \
    "$SRC" \
    -o "$EXE"
then
    echo
    echo "[Compile Error]"
    exit 1
fi

echo "[Compile] OK"

# ==========================================
# 公式サンプルを初回だけダウンロード
# ==========================================
if ! find "$SAMPLE_DIR" \
    -maxdepth 1 \
    -name "*.in" \
    -print -quit 2>/dev/null |
    grep -q .
then
    echo
    echo "[Download] サンプルを取得します"
    echo "$URL"

    rm -rf "$SAMPLE_DIR"
    mkdir -p "$SAMPLE_DIR"

    if ! oj download "$URL" -d "$SAMPLE_DIR"; then
        echo
        echo "サンプルの取得に失敗しました"
        echo "URL: $URL"
        exit 1
    fi
fi

# ==========================================
# デバッグ時のSanitizer設定
# ==========================================
if [[ "$MODE" == "debug" ]]; then
    export ASAN_OPTIONS="detect_leaks=1:halt_on_error=1"
    export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=1"
fi

echo
echo "[Test]"
echo "Time limit: $TIME_LIMIT per case"
echo

# 無限ループ対策として各ケースを5秒で停止
RUN_COMMAND="timeout -k 1s $TIME_LIMIT ./$EXE"

oj test \
    -d "$SAMPLE_DIR" \
    -c "$RUN_COMMAND"

STATUS=$?

echo

if [[ $STATUS -eq 0 ]]; then
    if [[ "$MODE" == "debug" ]]; then
        echo "[SUCCESS] デバッグテストも通過しました"
    fi
else
    echo "[FAILURE] テストに失敗しました(なんでやねん)"
fi

exit "$STATUS"
