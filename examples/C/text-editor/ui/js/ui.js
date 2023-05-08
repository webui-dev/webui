//

let About = document.getElementById("About");
let aboutBox = document.getElementById("about-box");

About.onclick = function() {
    // Open ABout
    aboutBox.style.display = "block";
}

window.onclick = function(event) {
    if (event.target == aboutBox) {
        // Close About
        aboutBox.style.display = "none";
    }
}
