/*
    WebUI Library 2.x
    TypeScript Dynamic Lib Example

    http://webui.me
    https://github.com/alifcommunity/webui
    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/  

import * as webui from "./webui.ts";

// Optional: Set the library path
webui.set_lib_path('../../../../build/Windows/MSVC/webui-2-x64.dll');

function check_password() {
    
	console.log("You clicked on the check password button...");
    // ...
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
	<button id="CheckPassword">Check Password</button> - <button id="Exit">Exit</button>
    </body>
</html>
`;

// Create new window
const my_window = webui.new_window();

// Bind
webui.bind(my_window, "Exit", function(){
    console.log("You clicked on the exit button...");
    webui.exit();
});
webui.bind(my_window, "CheckPassword", check_password);

// Show the window
if(!webui.show(my_window, my_html, webui.browser.chrome))
	webui.show(my_window, my_html, webui.browser.any);

// Wait until all windows get closed
await webui.wait();

console.log("Bye");
