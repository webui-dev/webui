// This file gets called like follow:
//
// 1. UI `Index.html` request:
//    `http://localhost:xxx/bun_test.ts?foo=123&bar=456`
//
// 2. WebUI runs command:
//    `bun run "bun_test.ts" "foo=123&bar=456"`
//
// 3. Bun parses args and prints the response

// Get Query (HTTP GET)
const args = process.argv.slice(2);
const query = args[0];

// Variables
let foo: string = '';
let bar: string = '';

// Read Query
const params = new URLSearchParams(query);
for (const [key, value] of params.entries()) {
	if (key === 'foo') foo = value; // 123
	else if (key === 'bar') bar = value; // 456
}

console.log('Response from Bun: ' + (parseInt(foo) + parseInt(bar))); // 579
