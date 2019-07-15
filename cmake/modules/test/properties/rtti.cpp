struct base {
    virtual ~base() {
    }
};

struct A : base {
};

struct B : base {
};

int main() {
    base *x = new A();
    B *y = dynamic_cast<B *>(x);
    if (y != 0) {
        return -1;
    } else {
        return 0;
    }
}
