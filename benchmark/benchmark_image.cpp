#include "../src/image/image.hpp"
#include "../src/image/image_io.hpp"
#include <benchmark/benchmark.h>

// 要 benchmark 的函数
static void BM_ImageNew(benchmark::State& state) {
  for (auto _ : state) {
    gray_image* img_gray = image_new(256, 256);
    image_delete(img_gray);
  }
}

static void BM_ColorImageNew(benchmark::State& state) {
  for (auto _ : state) {
    color_image* img_rgb = color_image_new(256, 256);
    color_image_delete(img_rgb);
  }
}

static void BM_ImageSetVal(benchmark::State& state) {
  gray_image* img_gray = image_new(256, 256);
  for (auto _ : state) {
    image_ones(img_gray);
    image_erase(img_gray);
  }
  image_delete(img_gray);
}

static void BM_ColorImageSetVal(benchmark::State& state) {
  color_image* img_rgb = color_image_new(256, 256);
  for (auto _ : state) {
    color_image_ones(img_rgb);
    color_image_erase(img_rgb);
  }
  color_image_delete(img_rgb);
}

// 这一句不能少
BENCHMARK(BM_ImageNew);
BENCHMARK(BM_ColorImageNew);
BENCHMARK(BM_ImageSetVal);
BENCHMARK(BM_ColorImageSetVal);

// Google Benchmark 提供的 main 函数
// 也可以直接在 target_link_libraries 中加入 benchmark::benchmark_main，这样就可以省略这一行代码了
BENCHMARK_MAIN();