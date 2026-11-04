extern void MUSTALIAS(int*, int*);

struct s{
	int *a;
	int b;
};

int main()
{
	struct s s1, s2;
	struct s * p1;
	int x, y;
	s1.a = &x;
	s1.b = y;
	s2 = s1;
	MUSTALIAS(s2.a, s1.a);
	return 0;
}
