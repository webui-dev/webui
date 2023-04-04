

// Run this script by:
// deno run --allow-all --unstable deno_server.ts

// Import WebUI module
import * as webui from "../module/webui.ts";

// Import Deno Server Module
import { serve } from "https://deno.land/std@0.158.0/http/server.ts";

// Optional - Set a custom library path:
//  const lib_full_path = '../../../../build/Windows/MSVC/webui-2-x64.dll';
//  console.log("Looking for the WebUI dynamic library at: " + lib_full_path);
//  webui.set_lib_path(lib_full_path);

// Deno Server Listener
const port = 8080;
const url = "http://localhost:" + 8080;
const handler = (request: Request): Response => {
  const body = `This is a Deno-Web-Server example. Your user-agent is:\n\n${
    request.headers.get("user-agent") ?? "Unknown"
  }`;
  return new Response(body, { status: 200 });
};
serve(handler, { port });

// Create new window
const my_window = webui.new_window();

// Show the window
if(!webui.open(my_window, url, webui.browser.chrome))
	webui.open(my_window, url, webui.browser.any);

// Wait until all windows get closed
await webui.wait();

// The window is closed.
console.log('Thank you.');
Deno.exit(0);
