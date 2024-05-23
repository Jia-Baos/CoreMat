#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <immintrin.h>

int main() {
  // malloc开辟内存，存储数据为随机数
  int *p1 = (int *)malloc(4 * sizeof(int));
  printf("p1's address: %p\n", p1);
  for (int n = 0; n < 4; ++n) {
    printf("p1[%d] = %d\n", n, p1[n]);
  }

  // // 对存储数据进行赋值
  // for (int n = 0; n < 4; ++n) {
  //   p1[n] = n * n;
  // }
  // for (int n = 0; n < 4; ++n) {
  //   printf("p1[%d] = %d\n", n, p1[n]);
  // }

  // calloc开辟内存，存储数据初始化为0
  int *p2 = (int *)calloc(4, sizeof(int));
  printf("p2's address: %p\n", p2);
  for (int n = 0; n < 4; ++n) {
    printf("p2[%d] = %d\n", n, p2[n]);
  }

  int arr[] = {1, 2, 3, 4};
  printf("arr's szie %d\n", sizeof(arr) / sizeof(int));
  int *p3 = (int *)malloc(4 * sizeof(int));
  printf("p3's address: %p\n", p3);
  memset(p3, 0x7F, 4 * sizeof(int));  // memset只能用来赋0
  for (int n = 0; n < 4; ++n) {
    printf("p3[%d] = %d\n", n, p3[n]);
  }

  p3 = (int *)realloc(p3, 8 * sizeof(int));
  printf("p3's address: %p\n", p3);
  memset(p3 + 4, 0, 4 * sizeof(int));
  for (int n = 0; n < 8; ++n) {
    printf("p3[%d] = %d\n", n, p3[n]);
  }

  // float 为四个字节，16字节对齐可以同时操作四个float
  int *p4 = (int *)_aligned_malloc(32 * sizeof(int), 16);
  _aligned_free(p4);

  printf("size of float %d\n", sizeof(double));

  free(p1);
  free(p2);
  free(p3);
}