#!/bin/sh

git clone \
  --depth 1  \
  --filter=blob:none  \
  --no-checkout \
  https://github.com/llvm/llvm-project

cd llvm-project
git checkout main -- pstl
