// Text Editor

// Elements
let About = document.getElementById("About");
let aboutBox = document.getElementById("about-box");

// About show
About.onclick = function() {
    // Open ABout
    aboutBox.style.display = "block";
}

// About hide
window.onclick = function(event) {
    if (event.target == aboutBox) {
        // Close About
        aboutBox.style.display = "none";
    }
}

// Create the editor
const editor = document.getElementById("editor");
const codeMirrorInstance = CodeMirror.fromTextArea(editor, {
    mode: "text/x-csrc",
    lineNumbers: true,
    tabSize: 4,
    indentUnit: 2,
    lineWrapping: true,
    theme: "lucario"
});

// Change editor language
function SetFileModeExtension(extension) {
    let mode = "";
    switch (extension) {
        case "js":
            mode = "text/javascript";
            break;
        case "c":
        case "cpp":
        case "h":
            mode = "text/x-csrc";
            break;
        case "py":
            mode = "text/x-python";
            break;
        case "html":
            mode = "text/html";
            break;
        default:
            mode = "text/x-csrc";
    }
    codeMirrorInstance.setOption("mode", mode);
}

// Add a line to the editor
function addLine(text) {
    const lastLine = codeMirrorInstance.lineCount();
    codeMirrorInstance.replaceRange(webui_decode(text) + "\n", {line: lastLine});

    const element = document.getElementById('SaveLi');
    element.style.color = '#ddecf9';
    element.style.pointerEvents = 'all';
}

// Add full text to the editor
function addText(text) {
    codeMirrorInstance.setValue(webui_decode(text));

    const element = document.getElementById('SaveLi');
    element.style.color = '#ddecf9';
    element.style.pointerEvents = 'all';
}

// Save the file
function SaveFile() {
    const content = codeMirrorInstance.getValue();
    webui_fn('Save', content);
}

window.addEventListener("DOMContentLoaded", (event) => {
    // Load
    codeMirrorInstance.setSize("100%", "99%");
});

function getFileExtension(path) {
    console.log('getFileExtension: ' + path);
    return path.split('.').pop();
}

function getFileName(path) {
    console.log('getFileName: ' + path);
    return path.split(/[/\\]/).pop();
}

function changeWindowTitle(newTitle) {
    document.title = newTitle;
}

// Set file title and language
function SetFile(path_base64) {
    const path = webui_decode(path_base64);
    const Extension = getFileExtension(path);
    const FileName = getFileName(path);
    console.log('Extension: ' + path);
    console.log('FileName: ' + path);
    SetFileModeExtension(Extension);
    changeWindowTitle(FileName);
}
