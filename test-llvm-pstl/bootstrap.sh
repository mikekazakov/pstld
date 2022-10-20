#!/bin/sh

git clone \
  --depth 1  \
  --filter=blob:none  \
  --no-checkout \
  https://github.com/llvm/llvm-project

cd llvm-project
git sparse-checkout init --cone
git sparse-checkout add pstl
git checkout
git apply ../utils.patch
