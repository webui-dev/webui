let query = Bun.argv[2];

// Variables
let foo: string = '';
let bar: string = '';

// Read Query
const params = new URLSearchParams(query);
for (const [key, value] of params.entries()) {
	if (key == 'foo') foo = value; // 123
	else if (key == 'bar') bar = value; // 456
}

console.error('foo + bar = ' + (parseInt(foo) + parseInt(bar))); // 579
