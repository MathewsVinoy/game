#!/bin/sh

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

glslc "$PROJECT_ROOT/engine/shaders/triangle.vert" \
      -o "$PROJECT_ROOT/engine/shaders/triangle.vert.spv"

glslc "$PROJECT_ROOT/engine/shaders/triangle.frag" \
      -o "$PROJECT_ROOT/engine/shaders/triangle.frag.spv"
