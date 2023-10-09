// Text Editor

// Elements
const saveBtn = document.getElementById('save-btn');
const aboutBox = document.getElementById('about-box');

// File Handling
const supportsFilePicker = 'showSaveFilePicker' in window;
let fileHandle;
let currentFile = { name: '', ext: '' };

// Setup Editor
const codeMirrorInstance = CodeMirror.fromTextArea(document.getElementById('editor'), {
	mode: 'text/x-csrc',
	lineNumbers: true,
	tabSize: 4,
	indentUnit: 2,
	lineWrapping: true,
	theme: 'lucario',
});

function setLanguage(extension) {
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

function setFile(file) {
	currentFile.name = file.name;
	currentFile.ext = file.name.split('.').pop();
	// Set file title and language in editor
	document.title = file.name;
	setLanguage(currentFile.ext);
}

function readFile(file) {
	const reader = new FileReader();
	// Add text to the editor
	reader.onload = (e) => codeMirrorInstance.setValue(e.target.result);
	reader.readAsText(file);
}

async function openFile() {
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

async function saveFile() {
	const content = codeMirrorInstance.getValue();
	if (supportsFilePicker) {
		if (fileHandle) {
			// Create a FileSystemWritableFileStream to write to
			const writableStream = await fileHandle.createWritable();
			await writableStream.write(content);
			// Write to disk
			await writableStream.close();
		} else {
			fileHandle = await showSaveFilePicker();
			saveFile();
			setFile(await fileHandle.getFile());
		}
	} else {
		// Download the file if using filePicker with a fileHandle for saving
		// is not supported by the browser. E.g., in Firefox.
		const blobData = new Blob([content], { type: 'text/${currentFile.ext}' });
		const urlToBlob = window.URL.createObjectURL(blobData);
		const a = document.createElement('a');
		a.style.setProperty('display', 'none');
		a.href = urlToBlob;
		a.download = document.title;
		a.click();
		window.URL.revokeObjectURL(urlToBlob);
		a.remove();
	}
	saveBtn.disabled = true;
}

// Navigation Events
// open
document.getElementById('open-btn').onclick = openFile;
// save
saveBtn.onclick = saveFile;
// about
document.getElementById('about-btn').onclick = () => (aboutBox.style.display = 'block'); // show
aboutBox.onclick = () => (aboutBox.style.display = 'none'); // hide

// Editor Events
// enable save on change
codeMirrorInstance.on('change', () => {
	saveBtn.disabled = false;
});
window.addEventListener('DOMContentLoaded', () => {
	codeMirrorInstance.setSize('100%', '99%');
});
