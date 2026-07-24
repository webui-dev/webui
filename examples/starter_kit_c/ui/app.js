// WebUI C Starter Kit - frontend logic
//
// Every C function bound with webui_bind() in src/main.c is available here
// as a global async JavaScript function.

// --- Backend demo call ------------------------------------------------------

async function callBackend() {
	const input = document.getElementById('name');
	const reply = document.getElementById('reply');
	const name = input.value.trim() || 'World';
	try {
		// Calls the C function greet() and waits for its response
		reply.textContent = await greet(name);
	} catch (err) {
		reply.textContent = 'Backend call failed: ' + err;
	}
}

document.getElementById('greet-btn').addEventListener('click', callBackend);
document.getElementById('name').addEventListener('keydown', (e) => {
	if (e.key === 'Enter') callBackend();
});

// --- Custom title bar (frameless window controls) ---------------------------

document.querySelector('#window-buttons .minimize').addEventListener('click', () => minimize());
document.querySelector('#window-buttons .maximize').addEventListener('click', () => maximize());
document.querySelector('#window-buttons .close').addEventListener('click', () => close_win());

// Double-click the title bar (not the buttons) to maximize/restore
document.getElementById('titlebar').addEventListener('dblclick', (e) => {
	if (!e.target.closest('#window-buttons')) maximize();
});
