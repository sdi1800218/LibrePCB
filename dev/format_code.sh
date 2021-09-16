#!/usr/bin/env bash
set -euo pipefail

# Formats files according our coding style.
#
# Usage:
#
#   format_code.sh [--all] [--sudo] [--docker]
#
# Notes:
#   - Run the command "./dev/format_code.sh" in the root of the repository.
#   - If you have Docker installed (on Linux), it's recommended to pass the
#     "--docker" parameter. A docker image containing all required tools will
#     then be created and used so you don't have to install any dependencies.
#   - To run docker with sudo, use the "--sudo" parameter.
#   - Without docker, make sure the executables "git" and "clang-format" are
#     available in PATH.
#   - To format all files (instead of only modified ones), add the "--all"
#     parameter. This is intended only for LibrePCB maintainers, usually you
#     should not use this!

DOCKER=""
DOCKER_CMD="docker"
CLANGFORMAT=${CLANGFORMAT:-clang-format}
ALL=""
for i in "$@"
do
case $i in
  --docker)
  DOCKER="--docker"
  shift
  ;;
  --sudo)
  DOCKER_CMD="sudo docker"
  shift
  ;;
  --all)
  ALL="--all"
  shift
  ;;
esac
done

REPO_ROOT=$(git rev-parse --show-toplevel)

if [ "$DOCKER" == "--docker" ]; then
  DOCKER_IMAGE=librepcb/devtools:1.0.0

  if [ "$($DOCKER_CMD images -q $DOCKER_IMAGE | wc -l)" == "0" ]; then
    echo "Building devtools container..."
    $DOCKER_CMD build "$REPO_ROOT/dev/devtools" -t librepcb/devtools:1.0.0
  fi

  echo "[Re-running format_code.sh inside Docker container]"
  $DOCKER_CMD run --rm -t --user "$(id -u):$(id -g)" \
    -v "$REPO_ROOT:/code" \
    $DOCKER_IMAGE \
    /bin/bash -c "cd /code && dev/format_code.sh $ALL"

  echo "[Docker done.]"
  exit 0
fi

COUNTER=0

# Format source files with clang-format.
clang_format_failed() {
  echo "" >&2
  echo "ERROR: clang-format failed!" >&2
  echo "  Make sure that clang-format 6 is installed." >&2
  echo "  On Linux, you can also run this script in a docker" >&2
  echo "  container by using the '--docker' argument." >&2
  exit 7
}
echo "Formatting sources with $CLANGFORMAT..."
for dir in apps/ libs/librepcb/ tests/unittests/
do
  if [ "$ALL" == "--all" ]; then
    TRACKED=$(git ls-files -- "${dir}**.cpp" "${dir}**.hpp" "${dir}**.h")
  else
    # Only files which differ from the master branch
    TRACKED=$(git diff --name-only master -- "${dir}**.cpp" "${dir}**.hpp" "${dir}**.h")
  fi
  UNTRACKED=$(git ls-files --others --exclude-standard -- "${dir}**.cpp" "${dir}**.hpp" "${dir}**.h")
  for file in $TRACKED $UNTRACKED
  do
    if [ -f "$file" ]; then
      # Note: Do NOT use in-place edition of clang-format because this causes
      # "make" to detect the files as changed every time, even if the content was
      # not modified! So we only overwrite the files if their content has changed.
      OLD_CONTENT=$(cat "$file")
      NEW_CONTENT=$($CLANGFORMAT -style=file "$file" || clang_format_failed)
      if [ "$NEW_CONTENT" != "$OLD_CONTENT" ]
      then
        printf "%s\n" "$NEW_CONTENT" > "$file"
        echo "[M] $file"
        COUNTER=$((COUNTER+1))
      else
        echo "[ ] $file"
      fi
    fi
  done
done

echo "Finished: $COUNTER files modified."
