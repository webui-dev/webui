// This file gets called like follow:
// 
// 1. UI `Index.html` request:
//    `http://localhost:xxx/deno_test.ts?foo=123&bar=456`
//
// 2. WebUI runs command:
//    `deno run --allow-all --unstable "deno_test.ts" "foo=123&bar=456"`
//
// 3. Deno parse args and print the response

// Import parse()
import { parse } from 'https://deno.land/std/flags/mod.ts';

// Get Query (HTTP GET)
const args = parse(Deno.args);
const query = args._[0] as string;

// Variables
let foo: string = '';
let bar: string = '';

// Read Query
const params = new URLSearchParams(query);
for (const [key, value] of params.entries()) {
	if (key == 'foo') foo = value; // 123
	else if (key == 'bar') bar = value; // 456
}

console.log('Response from Deno: ' + (parseInt(foo) + parseInt(bar))); // 579
