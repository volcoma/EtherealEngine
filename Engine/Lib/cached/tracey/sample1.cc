// tracey is callstack based. no dirty new/delete macro tricks.
// tracey is a static library. requires no source modification. just link it.

static int *static_cpp_leak = new int;

int main() {
    new int [400];
}
