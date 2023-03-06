/*
    WebUI Library 2.x
    TypeScript Dynamic Lib Example

    http://webui.me
    https://github.com/alifcommunity/webui
    Licensed under GNU General Public License v3.0.
    Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga>.
*/

// To run this script:
// deno run --allow-all --unstable example.ts 

import * as webui from "./webui.ts";

    // Optional: Set the library path
    // const lib_full_path = '../../../../build/Windows/MSVC/webui-2-x64.dll';
    // console.log("Optional: Looking for the WebUI dynamic library at: " + lib_full_path);
    // webui.set_lib_path(lib_full_path);

function test(e : webui.event) {
    
	console.log("You clicked on the test button...");
    
    console.log('test() -> element_id: ' + e.element_id);
    console.log('test() -> window_id: ' + e.window_id);
    console.log('test() -> element_name: ' + e.element_name);
    
    return "TEST";
}

const my_html = `
<!DOCTYPE html>
<html>
	<head>
		<title>WebUI 2 - Deno Dynamic Lib Example</title>
        <style>
            body {
                color: white;
                background: #0F2027;
                background: -webkit-linear-gradient(to right, #101314, #0f1c20, #061f2a);
                background: linear-gradient(to right, #101314, #0f1c20, #061f2a);
                text-align: center;
                font-size: 18px;
                font-family: sans-serif;
            }
        </style>
    </head>
    <body>
        <h1>WebUI 2 - Deno Dynamic Lib Example</h1>
        <br>
        <input type="password" id="MyInput" OnKeyUp="document.getElementById('err').innerHTML='&nbsp;';" autocomplete="off">
        <br>
	<h3 id="err" style="color: #dbdd52">&nbsp;</h3>
        <br>
	<button id="test">TEST</button> - <button id="Exit">Exit</button>
    </body>
</html>
`;

// Create new window
const my_window = webui.new_window();

// Bind
webui.bind(my_window, "Exit", function(e : webui.event){
    console.log("You clicked on the exit button...");
    webui.exit();
});
webui.bind(my_window, "test", test);

// Show the window
if(!webui.show(my_window, my_html, webui.browser.chrome))
	webui.show(my_window, my_html, webui.browser.any);

// Wait until all windows get closed
await webui.wait();

console.log("Bye");
