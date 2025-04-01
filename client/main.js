// Handles reload when manually refreshing browser
window.addEventListener("beforeunload", () => {
  const returnTo = localStorage.getItem("returnTo");
  if (!returnTo) {
    localStorage.removeItem("username");
  }
});

// Handles reload of page when logged in
window.addEventListener("DOMContentLoaded", () => {
  const username = localStorage.getItem("username");
  const returnTo = localStorage.getItem("returnTo");

  if (username) {
    document.getElementById("auth-section").style.display = "none";
    document.getElementById("navbar-section").style.display = "block";
    document.getElementById("userDisplay").textContent = username;

    if (returnTo === "current-books") {
      // Show current books
      document.getElementById("home-section").style.display = "none";
      document.getElementById("current-books-section").style.display = "block";

      // Trigger a refresh of the book list
      fetch(`http://localhost:8080/currentBooks?username=${username}`)
        .then(res => res.text())
        .then(html => {
          document.getElementById("checkedOutList").innerHTML = html;
        });

      // Clear flag
      localStorage.removeItem("returnTo");
    } else {
      // Default to home
      document.getElementById("home-section").style.display = "block";
    }
  }
});

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
          localStorage.setItem('username', username);  // Store for current-books.html
          document.getElementById("auth-section").style.display = "none";
          document.getElementById('navbar-section').style.display = 'block';
          document.getElementById("home-section").style.display = "block";
          document.getElementById('current-books-section').style.display = 'none';
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

  // Navbar navigation
  document.getElementById('navHome').addEventListener('click', (e) => {
    e.preventDefault();
    document.getElementById('home-section').style.display = 'block';
    document.getElementById('current-books-section').style.display = 'none';
  });

  document.getElementById('navCurrentBooks').addEventListener('click', (e) => {
    e.preventDefault();
    const username = localStorage.getItem('username');
    fetch(`http://localhost:8080/currentBooks?username=${username}`)
      .then(res => res.text())
      .then(html => {
        console.log("CurrentBooks HTML:", html);  
        document.getElementById('checkedOutList').innerHTML = html;
        document.getElementById('home-section').style.display = 'none';
        document.getElementById('current-books-section').style.display = 'block';
      })
      .catch(error => {
        console.error("Failed to fetch current books from nav:", error);
      });
  });  

  function checkInBook(bookID) {
    const username = localStorage.getItem('username');
  
    if (!username) {
      alert("Session expired. Please log in again.");
      return window.location.reload();
    }
  
    fetch(`http://localhost:8080/checkin?username=${username}&bookID=${bookID}`)
      .then(response => response.text())
      .then(msg => {
        alert(msg);
  
       
        console.log("Username before reloading currentBooks:", username);
        localStorage.setItem("returnTo", "current-books");


        fetch(`http://localhost:8080/currentBooks?username=${username}`)
          .then(res => {
            if (!res.ok) throw new Error(`Status: ${res.status}`);
            return res.text();
          })
          .then(html => {
            document.getElementById('checkedOutList').innerHTML = html;
            document.getElementById('home-section').style.display = 'none';
            document.getElementById('current-books-section').style.display = 'block';
          })
          .catch(err => {
            console.error("Reload currentBooks failed:", err);
            alert("Failed to update view. Please go to 'Current Books' again.");
          });
      })
      .catch(error => {
        console.error('Check-in error:', error);
        alert("Check-in failed. Please try again.");
      });
  }
  
  