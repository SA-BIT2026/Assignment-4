struct MyStruct {
	int * f1;
	int * f2;
};

int main() {
	struct MyStruct s[2];
	int a,b;
	s[0].f1 = &a;
	s[1].f1 = &b;

	return 0;
}
