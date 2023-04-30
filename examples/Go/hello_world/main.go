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
    Call Secret() function and get the response
    <br>
    <br>
    <button OnClick="webui_fn('Sec').then((response) => { alert('Response is ' + response) });">Secret</button>
    <br>
    <br>
    <button id="Exit">Exit</button>
  </body>
</html>`

func Exit(e webui.Event) string {

	webui.Exit()
	return ""
}

func Secret(e webui.Event) string {

	return "I Love Go!"
}

func Check(e webui.Event) string {

	// Create new JavaScript object
	js := webui.NewJavaScript()

	// Run the script
	if !webui.Script(e.Window, &js, "return document.getElementById('MyInput').value;") {

		// There is an error in our script
		fmt.Printf("JavaScript Error: %s\n", js.Response)
		return ""
	}

	fmt.Printf("Password: [%s]\n", js.Response)

	// Check the password
	if js.Response == "123456" {
		webui.Show(e.Window, dashboard_html)
	} else {
		webui.Script(e.Window, &js, "document.getElementById('err').innerHTML = 'Sorry. Wrong password';")
	}

	return ""
}

func main() {

	// New window
	var my_window = webui.NewWindow()

	// Bind
	webui.Bind(my_window, "CheckPassword", Check)
	webui.Bind(my_window, "Sec", Secret)
	webui.Bind(my_window, "Exit", Exit)

	// Show window
	webui.Show(my_window, login_html)

	// Loop
	webui.Wait()

	fmt.Println("Thank you.")
}
