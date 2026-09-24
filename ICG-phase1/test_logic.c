int main(){
	int a,b,r;
	a = 5;
	b = 10;

	r = a < b;
	println(r);

	r = a > b;
	println(r);

	r = a <= a;
	println(r);

	r = a >= b;
	println(r);

	r = a == a;
	println(r);

	r = a != b;
	println(r);

	r = a && b;
	println(r);

	r = a || 0;
	println(r);

	r = 0 || 0;
	println(r);

	r = !a;
	println(r);

	r = !0;
	println(r);

	return 0;
}
// 1
// 0
// 1
// 0
// 1
// 1
// 1
// 1
// 0
// 0
// 1