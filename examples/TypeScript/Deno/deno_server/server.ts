
// Modules
import { serve } from "https://deno.land/std@0.158.0/http/server.ts";

// Config
const port = 8080;
const url = "http://localhost:" + 8080;

// Server Listener
const handler = (request: Request): Response => {
  const body = `Your user-agent is:\n\n${
    request.headers.get("user-agent") ?? "Unknown"
  }`;

  return new Response(body, { status: 200 });
};

// Create a web server using Deno
serve(handler, { port });

// Run the WebUI example application to open a new window
const cmd = Deno.run({
    cmd: ["cmd", "/c", "window", url], 
    stdout: "piped",
    stderr: "piped"
});
const output = await cmd.output() // "piped" must be set
cmd.close();

// The window is closed.
console.log('Bye.');
Deno.exit(0);
