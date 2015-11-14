#include <cstdio>

namespace A {
  using foo = int;
}

int main() {
  A::foo a;

  a = 10;

  printf("%d\n", a);

  return 0;
}
