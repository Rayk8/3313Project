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
    document.getElementById("home-section").style.display = "block";

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
    } 
    else if (returnTo === "ratings") {
      document.getElementById("home-section").style.display = "none";
      document.getElementById("current-books-section").style.display = "none";
      document.getElementById("ratings-section").style.display = "block";
      document.getElementById("history-section").style.display = "none";
    
      fetch(`http://localhost:8080/ratings?username=${username}`)
        .then(res => res.text())
        .then(html => {
          document.getElementById("ratingsList").innerHTML = html;
        });
    
      localStorage.removeItem("returnTo");
    }    
    else if (returnTo === "home") {
      document.getElementById("home-section").style.display = "block";
      document.getElementById("current-books-section").style.display = "none";
      document.getElementById("ratings-section").style.display = "none";
      document.getElementById("history-section").style.display = "none";
    
      fetch("http://localhost:8080/catalog")
        .then(res => res.text())
        .then(html => {
          document.getElementById("availableBooksList").innerHTML = html;
        });
    
      localStorage.removeItem("returnTo");
    }    
    else {
      // Default to home
      document.getElementById("home-section").style.display = "block";
      fetch("http://localhost:8080/catalog")
        .then(res => res.text())
        .then(html => {
          console.log("Available books HTML:", html);
          document.getElementById("availableBooksList").innerHTML = html;
        })
        .catch(err => {
          console.error("Failed to load available books on reload:", err);
        });
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
        fetch("http://localhost:8080/catalog")
  .then(res => res.text())
  .then(html => {
    console.log("Catalog HTML after login:", html);
    document.getElementById("availableBooksList").innerHTML = html;
  })
  .catch(err => {
    console.error("Failed to load catalog after login:", err);
  });
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
  document.getElementById("home-section").style.display = "block";
  document.getElementById("current-books-section").style.display = "none";
  document.getElementById("ratings-section").style.display = "none";
  document.getElementById("history-section").style.display = "none";

    fetch("http://localhost:8080/catalog")
    .then(res => res.text())
    .then(html => {
      document.getElementById("availableBooksList").innerHTML = html;
    })
    .catch(err => {
      console.error("Failed to reload catalog on Home click:", err);
    });

  });

  document.getElementById('navCurrentBooks').addEventListener('click', (e) => {
    e.preventDefault();
    const username = localStorage.getItem('username');
    fetch(`http://localhost:8080/currentBooks?username=${username}`)
      .then(res => res.text())
      .then(html => {
        console.log("CurrentBooks HTML:", html);  
      document.getElementById("checkedOutList").innerHTML = html;
      document.getElementById("home-section").style.display = "none";
      document.getElementById("current-books-section").style.display = "block";
      document.getElementById("ratings-section").style.display = "none";
      document.getElementById("history-section").style.display = "none";
      })
      .catch(error => {
        console.error("Failed to fetch current books from nav:", error);
      });
  });  
  function checkoutBook(bookID) {
    const username = localStorage.getItem("username");
  
    if (!username) {
      alert("Please log in first.");
      return;
    }

    localStorage.setItem("returnTo", "home");
  
    fetch(`http://localhost:8080/checkout?username=${username}&bookID=${bookID}`)
      .then(res => res.text())
      .then(msg => {
        alert(msg);
  
       
        fetch("http://localhost:8080/catalog")
          .then(res => res.text())
          .then(html => {
            document.getElementById("availableBooksList").innerHTML = html;
          });
  
        
        fetch(`http://localhost:8080/currentBooks?username=${username}`)
          .then(res => res.text())
          .then(html => {
            document.getElementById("checkedOutList").innerHTML = html;
          });
      })
      .catch(err => {
        console.error("Checkout error:", err);
        alert("Failed to borrow the book.");
      });
  }
  

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
  
            return fetch("http://localhost:8080/catalog");
          })
          .then(res => res.text())
          .then(html => {
            document.getElementById("availableBooksList").innerHTML = html;
          })
          .catch(err => {
            console.error("Reload currentBooks or catalog failed:", err);
            alert("Failed to update views. Please refresh.");
          });
      })
      .catch(error => {
        console.error('Check-in error:', error);
        alert("Check-in failed. Please try again.");
      });
  }
  
  function submitRating(bookID) {
    const username = localStorage.getItem("username");
    const select = document.getElementById(`rating_${bookID}`);
    const rating = select.value;
  
    if (!rating) {
      alert("Please select a rating before submitting.");
      return;
    }
  
    localStorage.setItem("returnTo", "ratings");
  
    fetch(`http://localhost:8080/rate?username=${username}&bookID=${bookID}&rating=${rating}`)
      .then(res => res.text())
      .then(msg => {
        alert(msg);
  
      })
      .catch(err => {
        console.error("Rating error:", err);
        alert("Failed to submit rating.");
      });
  }

  document.getElementById("navRatings").addEventListener("click", (e) => {
    e.preventDefault();
    const username = localStorage.getItem("username");
  
    document.getElementById("home-section").style.display = "none";
    document.getElementById("current-books-section").style.display = "none";
    document.getElementById("ratings-section").style.display = "none";
  document.getElementById("history-section").style.display = "none";

    fetch(`http://localhost:8080/ratings?username=${username}`)
      .then(res => res.text())
      .then(html => {
        document.getElementById("ratingsList").innerHTML = html;
        document.getElementById("ratings-section").style.display = "block";
      })
      .catch(err => {
        console.error("Failed to load ratings:", err);
        alert("Could not load ratings.");
      });
  });
  
document.getElementById("navHistory").addEventListener("click", (e) => {
  e.preventDefault();
  const username = localStorage.getItem("username");

  document.getElementById("home-section").style.display = "none";
  document.getElementById("current-books-section").style.display = "none";
  document.getElementById("ratings-section").style.display = "none";
  document.getElementById("history-section").style.display = "none";

  fetch(`http://localhost:8080/history?username=${username}`)
    .then((res) => res.text())
    .then((html) => {
      document.getElementById("historyList").innerHTML = html;
      document.getElementById("history-section").style.display = "block";
    })
    .catch((err) => {
      console.error("Failed to load history:", err);
      alert("Could not load book history.");
    });
});
  