

mod Webui;

fn close_the_application (_e: Webui::Event) {

    Webui::Exit();
}

fn check_the_password (e: Webui::Event) {

    // Script to get the text value
    let mut js = Webui::JavaScript {
        timeout: 10,
        script: "return document.getElementById(\"MyInput\").value;".to_string(),
        data: "".to_string(),
        error: false,
    };

    // Run the script
    Webui::RunJavaScript(e.Window, &mut js);

    // Check if any error
    if !js.error {

        // Check the password
        println!("Password: {}", js.data);
        if js.data == "123456" {
            
            // Password is correct
            js.script = "alert('Good. The password is correct');".to_string();
            Webui::RunJavaScript(e.Window, &mut js);
        }
        else {

            // Wrong password
            js.script = "alert('Sorry. Wrong password');".to_string();
            Webui::RunJavaScript(e.Window, &mut js);
        }
    }
    else {

        // There is an error in our script
        println!("JavaScript Error: {}", js.data);
    }
}

fn main() {

    // UI HTML
    let my_html = "<!DOCTYPE html>
    <html><head><title>WebUI 2 - Rust Example</title>
    <style>body{color: white; background: #0F2027;
    background: -webkit-linear-gradient(to right, #2a4c4c, #0b181c, #020f14);
    background: linear-gradient(to right, #2a4c4c, #0b181c, #020f14);
    text-align:center; font-size: 18px; font-family: sans-serif;}</style></head><body>
    <h1>WebUI 2 - Rust Example</h1><br>
    <input type=\"password\" id=\"MyInput\"><br><br>
    <button id=\"MyButton1\">Check Password</button> - <button id=\"MyButton2\">Exit</button>
    </body></html>";

    // Create new window
    let my_window = Webui::NewWindow();

    // Bind an HTML element with a function
    Webui::Bind(my_window, "MyButton1", check_the_password);
    Webui::Bind(my_window, "MyButton2", close_the_application);

    // Show the window
    Webui::Show(my_window, my_html);

    // Wait until all windows get closed
    Webui::Wait();        

    println!("Thank you.");
}
