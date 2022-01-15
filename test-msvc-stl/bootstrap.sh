#!/bin/sh

git clone \
  --depth 1  \
  --filter=blob:none  \
  --no-checkout \
  https://github.com/microsoft/STL.git

cd STL
git sparse-checkout init --cone
git sparse-checkout add tests
git checkout
git apply ../transform_exclusive_scan.patch
