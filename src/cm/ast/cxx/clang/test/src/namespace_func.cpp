
// Test for parsing simple function in namespace

namespace ns {
    float foo(int x, int y);
}

float ns::foo(int x, int y) {
    return 0.0f;
}
