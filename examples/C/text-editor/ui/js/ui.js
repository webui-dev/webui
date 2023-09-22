// Text Editor

// Elements
let About = document.getElementById("About");
let aboutBox = document.getElementById("about-box");
let fileHandle = null;

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

// Add full text to the editor
function addText(text) {
    codeMirrorInstance.setValue(text);

    const element = document.getElementById('SaveLi');
    element.style.color = '#ddecf9';
    element.style.pointerEvents = 'all';
}

async function OpenFile() {
    [fileHandle] = await showOpenFilePicker({ multiple: false });
    const fileData = await fileHandle.getFile();

    // Read File
    const reader = new FileReader();
    reader.onload = (e) => addText(e.target.result);
    reader.readAsText(fileData);

    // Set file title and language
    document.title = fileData.name;
    SetFileModeExtension(fileData.name.split('.').pop());
}

async function SaveFile() {
    // Create a FileSystemWritableFileStream to write to
    const writableStream = await fileHandle.createWritable();
    const content = codeMirrorInstance.getValue();
    await writableStream.write(content);

    // Write to disk
    await writableStream.close();
}

window.addEventListener("DOMContentLoaded", (event) => {
    codeMirrorInstance.setSize("100%", "99%");
});
