int main(){
	int a,b;
	a = 5;

	b = -a;
	println(b);

	b = -(-a);
	println(b);

	b = +a;
	println(b);

	a++;
	println(a);

	a++;
	println(a);

	a--;
	println(a);

	return 0;
}
// -5
// 5
// 5
// 6
// 7
// 6
