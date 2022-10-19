// WebUI Go Example
// https://github.com/alifcommunity/webui

package main

import (
	"fmt"

	"github.com/alifcommunity/webui/webui"
)

const my_html string = `<!DOCTYPE html>
<html>
  <head>
    <title>WebUI 2 - Go Example</title>
    <style>
      body {
        color: white;
        background: #0F2027;
        background: -webkit-linear-gradient(to right, #2C5364, #203A43, #0F2027);
        background: linear-gradient(to right, #2C5364, #203A43, #0F2027);
        text-align: center;
        font-size: 18px;
        font-family: sans-serif;
      }
    </style>
  </head>
  <body>
    <h1>WebUI 2 - Go Example</h1>
    <br>
    <input type="password" id="MyInput">
    <br>
    <br>
	<button id="CheckPassword">Check Password</button> - <button id="Exit">Exit</button>
  </body>
</html>`

func Exit(e webui.Event) {

	webui.Exit()
}

func Check(e webui.Event) {

	// Script to get the text value
	js := webui.JavaScript{
		Timeout: 10,
		Script:  "return document.getElementById(\"MyInput\").value;",
	}

	// Run the script
	webui.RunJavaScript(e.Window, &js)

	// Check if any error
	if !js.Error {

		fmt.Printf("Password: %s\n", js.Data)

		// Check the password
		if js.Data == "123456" {

			js.Script = "alert('Good. Password is correct.')"
			webui.RunJavaScript(e.Window, &js)
		} else {

			js.Script = "alert('Sorry. Wrong password.')"
			webui.RunJavaScript(e.Window, &js)
		}
	} else {

		// There is an error in our script
		fmt.Printf("JavaScript Error: %s\n", js.Data)
	}
}

func main() {

	// New window
	var my_window = webui.NewWindow()

	// Bind
	webui.Bind(my_window, "CheckPassword", Check)
	webui.Bind(my_window, "Exit", Exit)

	// Show window
	webui.Show(my_window, my_html, 0)

	// Loop
	webui.Loop()

	fmt.Println("Bye.")
}
