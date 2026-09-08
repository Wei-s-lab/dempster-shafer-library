#!/usr/bin/env bash
# Smoke-test that an external CMake project can consume ds::ds via
# add_subdirectory and via find_package, without pulling in examples.
set -euo pipefail

: "${DS_SOURCE_DIR:?DS_SOURCE_DIR must be set to the library source root}"
: "${DS_BINARY_DIR:?DS_BINARY_DIR must be set to the library build tree}"
: "${CMAKE_COMMAND:?CMAKE_COMMAND must be set}"

WORKDIR="${DS_BINARY_DIR}/consumer_smoke"
PREFIX="${WORKDIR}/prefix"
ADD_SUB_BUILD="${WORKDIR}/add_subdirectory"
FIND_PKG_BUILD="${WORKDIR}/find_package"

rm -rf "${WORKDIR}"
mkdir -p "${WORKDIR}"

echo "==> [1/2] add_subdirectory consumer"
"${CMAKE_COMMAND}" -S "${DS_SOURCE_DIR}/tests/consumer/add_subdirectory" \
    -B "${ADD_SUB_BUILD}" \
    -DDS_ROOT="${DS_SOURCE_DIR}" \
    -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE:-Debug}"
"${CMAKE_COMMAND}" --build "${ADD_SUB_BUILD}" -j
"${ADD_SUB_BUILD}/consumer_main"

echo "==> [2/2] install + find_package consumer"
"${CMAKE_COMMAND}" --install "${DS_BINARY_DIR}" --prefix "${PREFIX}"
"${CMAKE_COMMAND}" -S "${DS_SOURCE_DIR}/tests/consumer/find_package" \
    -B "${FIND_PKG_BUILD}" \
    -DCMAKE_PREFIX_PATH="${PREFIX}" \
    -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE:-Debug}"
"${CMAKE_COMMAND}" --build "${FIND_PKG_BUILD}" -j
"${FIND_PKG_BUILD}/consumer_main"

echo "consumer smoke tests passed"
