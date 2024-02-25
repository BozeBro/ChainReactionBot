#include <cstdlib>
#include <iostream>
using namespace std;
int funStuff(int a, int b) {
  int c = a * b * b - a;
  int d = a + b + 1;
  if (c == 0) {
    b = 2 * b;
    d = d - b + 1;
    if (a == 0) {
      std::cout << d;
    } else {
      d = d - a - 1;
      std::cout << d;
    }
  } else {
    int e = c + 3 * a * b * b;
    if (e == 0) {
      b = 2 * b;
      std::cout << b;
    } else {
      a = a + 1;
      std::cout << a;
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  int a = atoi(argv[1]);
  int b = atoi(argv[2]);
  funStuff(a, b);
  return 0;
}
