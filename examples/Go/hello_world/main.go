package main

import (
	"fmt"

	"github.com/alifcommunity/webui/webui"
)

const login_html string = `<!DOCTYPE html>
<html>
  <head>
    <title>WebUI 2 - Go Example</title>
    <style>
      body {
        color: white;
        background: #0F2027;
        background: -webkit-linear-gradient(to right, #4e99bb, #2c91b5, #07587a);
        background: linear-gradient(to right, #4e99bb, #2c91b5, #07587a);
        text-align: center;
        font-size: 18px;
        font-family: sans-serif;
      }
    </style>
  </head>
  <body>
    <h1>WebUI 2 - Go Example</h1>
    <br>
    <input type="password" id="MyInput" OnKeyUp="document.getElementById('err').innerHTML='&nbsp;';" autocomplete="off">
    <br>
    <h3 id="err" style="color: #dbdd52">&nbsp;</h3>
    <br>
    <button id="CheckPassword">Check Password</button> - <button id="Exit">Exit</button>
  </body>
</html>`

const dashboard_html string = `<!DOCTYPE html>
<html>
  <head>
    <title>Dashboard</title>
    <style>
      body {
        color: white;
        background: #0F2027;
        background: -webkit-linear-gradient(to right, #4e99bb, #2c91b5, #07587a);
        background: linear-gradient(to right, #4e99bb, #2c91b5, #07587a);
        text-align: center;
        font-size: 18px;
        font-family: sans-serif;
      }
    </style>
  </head>
  <body>
    <h1>Welcome !</h1>
    <br>
    <br>
    <button id="Exit">Exit</button>
  </body>
</html>`

func Exit(e webui.Event) {

	webui.Exit()
}

func Check(e webui.Event) {

	// Script to get the text value
	MyScript := webui.JavaScript{
		Timeout: 10,
		Script:  " return document.getElementById('MyInput').value; ",
	}

	// Run the script
	webui.RunJavaScript(e.Window, &MyScript)

	// Check if any error
	if !MyScript.Error {

		fmt.Printf("Password: %s\n", MyScript.Data)

		// Check the password
		if MyScript.Data == "123456" {

			webui.Show(e.Window, dashboard_html)
		} else {

			MyScript.Script = " document.getElementById('err').innerHTML = 'Sorry. Wrong password'; "
			webui.RunJavaScript(e.Window, &MyScript)
		}
	} else {

		// There is an error in our script
		fmt.Printf("JavaScript Error: %s\n", MyScript.Data)
	}
}

func main() {

	// New window
	var my_window = webui.NewWindow()

	// Bind
	webui.Bind(my_window, "CheckPassword", Check)
	webui.Bind(my_window, "Exit", Exit)

	// Show window
	webui.Show(my_window, login_html)

	// Loop
	webui.Wait()

	fmt.Println("Thank you.")
}
