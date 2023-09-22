// Text Editor

// Elements
const aboutBtn = document.getElementById('about-button');
const aboutBox = document.getElementById('about-box');
const saveBtn = document.getElementById('save-button');
const supportsFilePicker = 'showSaveFilePicker' in window;
let fileHandle;
let openFile = {
    name: '',
    ext: '',
};

// About show
aboutBtn.onclick = () => (aboutBox.style.display = 'block');
// About hide
aboutBox.onclick = () => (aboutBox.style.display = 'none');

// Create the editor
const editor = document.getElementById('editor');
const codeMirrorInstance = CodeMirror.fromTextArea(editor, {
    mode: 'text/x-csrc',
    lineNumbers: true,
    tabSize: 4,
    indentUnit: 2,
    lineWrapping: true,
    theme: 'lucario',
});

// Change editor language
function SetFileModeExtension(extension) {
    let mode = '';
    switch (extension) {
        case 'js':
            mode = 'text/javascript';
            break;
        case 'c':
        case 'cpp':
        case 'h':
            mode = 'text/x-csrc';
            break;
        case 'py':
            mode = 'text/x-python';
            break;
        case 'html':
            mode = 'text/html';
            break;
        default:
            mode = 'text/x-csrc';
    }
    codeMirrorInstance.setOption('mode', mode);
}

// Add full text to the editor
function addText(text) {
    codeMirrorInstance.setValue(text);

    saveBtn.style.color = '#ddecf9';
    saveBtn.style.pointerEvents = 'all';
}

function readFile(file) {
    const reader = new FileReader();
    reader.onload = (e) => addText(e.target.result);
    reader.readAsText(file);
}

function setFile(file) {
    openFile.name = file.name;
    openFile.ext = file.name.split('.').pop();
    // Set file title and language in editor
    document.title = file.name;
    SetFileModeExtension(openFile.ext);
}

async function OpenFile() {
    if (supportsFilePicker) {
        [fileHandle] = await showOpenFilePicker({ multiple: false });
        fileData = await fileHandle.getFile();
        readFile(fileData);
        setFile(fileData);
    } else {
        let input = document.createElement('input');
        input.type = 'file';
        input.onchange = (e) => {
            readFile(e.target.files[0]);
            setFile(e.target.files[0]);
        };
        input.click();
        input.remove();
    }
}

async function SaveFile() {
    const content = codeMirrorInstance.getValue();
    if (supportsFilePicker && fileHandle) {
        // Create a FileSystemWritableFileStream to write to
        const writableStream = await fileHandle.createWritable();
        await writableStream.write(content);
        // Write to disk
        await writableStream.close();
    } else {
        // Download the file if using filePicker with a fileHandle for saving
        // is not supported by the browser. E.g., in Firefox.
        const blobData = new Blob([content], { type: 'text/${openFile.ext}' });
        const urlToBlob = window.URL.createObjectURL(blobData);
        const a = document.createElement('a');
        a.style.setProperty('display', 'none');
        a.href = urlToBlob;
        a.download = document.title;
        a.click();
        window.URL.revokeObjectURL(urlToBlob);
        a.remove();
    }
}

window.addEventListener('DOMContentLoaded', () => {
    codeMirrorInstance.setSize('100%', '99%');
});
