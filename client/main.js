// Registration form handler
document.getElementById("registerForm").addEventListener("submit", function(event) {
    event.preventDefault();
    const username = document.getElementById("regUsername").value;
    const password = document.getElementById("regPassword").value;
    fetch(`http://localhost:8080/register?username=${encodeURIComponent(username)}&password=${encodeURIComponent(password)}`)
      .then(response => response.text())
      .then(data => {
        console.log("Registration response:", data);
        document.getElementById("registerResult").innerHTML = data;
      })
      .catch(error => {
        console.error("Registration error:", error);
      });
  });
  
  // Login form handler
  document.getElementById("loginForm").addEventListener("submit", function(event) {
    event.preventDefault();
    const username = document.getElementById("username").value;
    const password = document.getElementById("password").value;
    fetch(`http://localhost:8080/login?username=${encodeURIComponent(username)}&password=${encodeURIComponent(password)}`)
      .then(response => response.text())
      .then(data => {
        console.log("Server response:", data);
        document.getElementById("loginResult").innerHTML = data;
        if (data.includes("Login Successful")) {
          // Hide the authentication section and show the home (search) section.
          document.getElementById("auth-section").style.display = "none";
          document.getElementById("home-section").style.display = "block";
          document.getElementById("userDisplay").textContent = username;
        }
      })
      .catch(error => {
        console.error("Login error:", error);
      });
  });
  
  // Search button handler
  document.getElementById("searchButton").addEventListener("click", function() {
    const query = document.getElementById("searchQuery").value;
    fetch(`http://localhost:8080/search?query=${encodeURIComponent(query)}`)
      .then(response => response.text())
      .then(data => {
        document.getElementById("searchResults").innerHTML = data;
      })
      .catch(error => {
        console.error("Search error:", error);
      });
  });
  