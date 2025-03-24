document.getElementById('loginForm').addEventListener('submit', function(e) {
    e.preventDefault();
    // For now, assume any non-empty credentials are valid.
    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;

    if(username && password) {
        // Hide the login form and show the catalog area.
        document.getElementById('login').style.display = 'none';
        document.getElementById('content').style.display = 'block';
        // Here you could later call the server to retrieve the catalog.
    }
});
