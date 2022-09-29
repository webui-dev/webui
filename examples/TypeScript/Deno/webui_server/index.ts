
console.log(
	"Welcome!, You are using a WebUI server.\nImporting arithmetic.ts module..."
);

import { add, multiply } from "./arithmetic.ts";

function totalCost(outbound: number, inbound: number, tax: number): number {
	
  return multiply(add(outbound, inbound), tax);
}

console.log(totalCost(19, 31, 1.2));
console.log(totalCost(45, 27, 1.15));
