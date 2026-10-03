import Register from "./pages/Register";
import { useEffect, useState } from "react";
import Login from "./pages/Login";
import "./App.css";

function App() {
  const toggleTheme = () => {
    const newMode = !darkMode;

    setDarkMode(newMode);
    localStorage.setItem("darkMode", newMode);
  };
  const [isLoggedIn, setIsLoggedIn] = useState(
    localStorage.getItem("isLoggedIn") === "true",
  );
  const [username, setUsername] = useState(
    localStorage.getItem("username") || "",
  );
  const [showRegister, setShowRegister] = useState(false);
  const [currentRoom, setCurrentRoom] = useState("Lobby");

  const [activeChat, setActiveChat] = useState({
    type: "room",
    name: "Lobby",
  });

  const [messageText, setMessageText] = useState("");
  const [searchText, setSearchText] = useState("");
  const [notification, setNotification] = useState("");
  const [showProfile, setShowProfile] = useState(false);

  const [darkMode, setDarkMode] = useState(
    localStorage.getItem("darkMode") === "true",
  );

  useEffect(() => {
    if (!notification) {
      return;
    }

    const timer = setTimeout(() => {
      setNotification("");
    }, 3000);

    return () => clearTimeout(timer);
  }, [notification]);

  const [unreadCounts, setUnreadCounts] = useState({
    Lobby: 0,
    Java: 2,
    Python: 1,
    Rahul: 3,
    Amit: 1,
  });

  const [roomMessages, setRoomMessages] = useState({
    Lobby: [
      {
        username: "Rahul",
        text: "Hello everyone!",
        time: "7:45 PM",
        own: false,
      },
      {
        username,
        text: "Hi Rahul 👋",
        time: "7:46 PM",
        own: true,
      },
    ],

    Java: [
      {
        username: "Amit",
        text: "Welcome to the Java room!",
        time: "7:50 PM",
        own: false,
      },
    ],

    Python: [
      {
        username: "Rahul",
        text: "Anyone learning Python?",
        time: "7:55 PM",
        own: false,
      },
    ],
  });

  const [privateMessages, setPrivateMessages] = useState({
    Rahul: [
      {
        username: "Rahul",
        text: "Hey Naresh!",
        time: "8:00 PM",
        own: false,
      },
      {
        username: "Naresh",
        text: "Hi Rahul 👋",
        time: "8:01 PM",
        own: true,
      },
    ],

    Amit: [
      {
        username: "Amit",
        text: "Are you working on the project?",
        time: "8:05 PM",
        own: false,
      },
    ],
  });

  const messages =
    activeChat.type === "private"
      ? privateMessages[activeChat.name]
      : roomMessages[currentRoom];

  const filteredMessages = messages.filter((message) =>
    message.text.toLowerCase().includes(searchText.toLowerCase()),
  );

  const roomUsers = {
    Lobby: [username, "Rahul", "Amit"],
    Java: [username, "Amit"],
    Python: [username, "Rahul"],
  };
  const roomDescriptions = {
    Lobby: "General chat for everyone",
    Java: "Java programming discussion",
    Python: "Python programming discussion",
  };
  const directUsers = ["Rahul", "Amit"];
  const sendMessage = () => {
    if (messageText.trim() === "") {
      return;
    }

    const newMessage = {
      username,
      text: messageText,
      time: new Date().toLocaleTimeString([], {
        hour: "2-digit",
        minute: "2-digit",
      }),
      own: true,
    };

    setRoomMessages({
      ...roomMessages,
      [currentRoom]: [...messages, newMessage],
    });
    setMessageText("");
  };

  if (!isLoggedIn) {
    if (showRegister) {
      return (
        <Register
          onRegister={(registeredUsername) => {
            setUsername(registeredUsername);
            setIsLoggedIn(true);
            setShowRegister(false);

            localStorage.setItem("isLoggedIn", "true");
            localStorage.setItem("username", registeredUsername);
          }}
          onBackToLogin={() => setShowRegister(false)}
        />
      );
    }

    return (
      <Login
        onLogin={(loggedInUsername) => {
          setUsername(loggedInUsername);
          setIsLoggedIn(true);

          localStorage.setItem("isLoggedIn", "true");
          localStorage.setItem("username", loggedInUsername);
        }}
        onRegister={() => setShowRegister(true)}
      />
    );
  }
  return (
    <div className={`chat-app ${darkMode ? "dark-mode" : ""}`}>
      {/* Header */}
      <header className="chat-header">
        <div>
          <h1>Linux Chat Application</h1>
          <span>TCP Multi-Client Chat</span>
        </div>
        <div className="header-actions">
          <button className="theme-button" onClick={toggleTheme}>
            {darkMode ? "☀️" : "🌙"}
          </button>

          <div className="profile-container">
            <button
              className="profile-button"
              onClick={() => setShowProfile(!showProfile)}
            >
              👤 {username} ▾
            </button>

            {showProfile && (
              <div className="profile-menu">
                <div className="profile-name">👤 {username}</div>

                <div className="profile-status">🟢 Online</div>

                <button
                  className="profile-logout"
                  onClick={() => {
                    setIsLoggedIn(false);
                    setUsername("");
                    setShowProfile(false);

                    localStorage.removeItem("isLoggedIn");
                    localStorage.removeItem("username");
                  }}
                >
                  Logout
                </button>
              </div>
            )}
          </div>
        </div>
      </header>

      {/* Main Layout */}
      <main className="chat-layout">
        {/* Rooms */}
        <aside className="rooms-sidebar">
          <h2>Rooms</h2>

          <button
            className={`room ${currentRoom === "Lobby" ? "active" : ""}`}
            onClick={() => {
              setCurrentRoom("Lobby");
              setActiveChat({
                type: "room",
                name: "Lobby",
              });
            }}
          >
            🏠 Lobby
          </button>

          <button
            className={`room ${currentRoom === "Java" ? "active" : ""}`}
            onClick={() => {
              setCurrentRoom("Java");

              setActiveChat({
                type: "room",
                name: "Java",
              });

              setUnreadCounts({
                ...unreadCounts,
                Java: 0,
              });
            }}
          >
            <span>☕ Java</span>

            {unreadCounts.Java > 0 && (
              <span className="unread-count">{unreadCounts.Java}</span>
            )}
          </button>

          <button
            className={`room ${currentRoom === "Python" ? "active" : ""}`}
            onClick={() => {
              setCurrentRoom("Python");

              setActiveChat({
                type: "room",
                name: "Python",
              });

              setUnreadCounts({
                ...unreadCounts,
                Python: 0,
              });
            }}
          >
            <span>🐍 Python</span>

            {unreadCounts.Python > 0 && (
              <span className="unread-count">{unreadCounts.Python}</span>
            )}
          </button>
          <h2 className="direct-title">Direct Messages</h2>

          {directUsers.map((user) => (
            <button
              className={`room ${
                activeChat.type === "private" && activeChat.name === user
                  ? "active"
                  : ""
              }`}
              key={user}
              onClick={() => {
                setActiveChat({
                  type: "private",
                  name: user,
                });

                setUnreadCounts({
                  ...unreadCounts,
                  [user]: 0,
                });
              }}
            >
              <span>🟢 {user}</span>

              {unreadCounts[user] > 0 && (
                <span className="unread-count">{unreadCounts[user]}</span>
              )}
            </button>
          ))}
        </aside>

        {/* Chat */}
        <section className="chat-section">
          <div className="chat-title">
            <div>
              <h2>
                {activeChat.type === "private"
                  ? `Private Chat with ${activeChat.name}`
                  : activeChat.name}
              </h2>

              <span>
                {activeChat.type === "private"
                  ? "Private conversation"
                  : roomDescriptions[currentRoom]}
              </span>
            </div>

            <div className="chat-title-right">
              <span>
                {activeChat.type === "private"
                  ? "Private"
                  : `${roomUsers[currentRoom].length} users online`}
              </span>

              {activeChat.type === "room" && activeChat.name !== "Lobby" && (
                <button
                  className="leave-room-button"
                  onClick={() => {
                    setCurrentRoom("Lobby");

                    setActiveChat({
                      type: "room",
                      name: "Lobby",
                    });
                  }}
                >
                  Leave Room
                </button>
              )}
            </div>
          </div>
          {notification && <div className="notification">{notification}</div>}

          <div className="message-search">
            <input
              type="text"
              placeholder="Search messages..."
              value={searchText}
              onChange={(event) => setSearchText(event.target.value)}
            />
          </div>

          <div className="messages">
            {filteredMessages.map((message, index) => (
              <div
                className={`message ${message.own ? "own" : ""}`}
                key={index}
              >
                <div className="message-avatar">
                  {message.username.charAt(0).toUpperCase()}
                </div>

                <div className="message-content">
                  <strong>{message.username}</strong>
                  <p>{message.text}</p>
                  <small>{message.time}</small>
                </div>
              </div>
            ))}
          </div>

          <div className="message-input">
            <input
              type="text"
              placeholder="Type a message..."
              value={messageText}
              onChange={(event) => setMessageText(event.target.value)}
              onKeyDown={(event) => {
                if (event.key === "Enter") {
                  const sendMessage = () => {
                    if (messageText.trim() === "") {
                      return;
                    }

                    const newMessage = {
                      username,
                      text: messageText,
                      time: new Date().toLocaleTimeString([], {
                        hour: "2-digit",
                        minute: "2-digit",
                      }),
                      own: true,
                    };

                    if (activeChat.type === "private") {
                      setPrivateMessages({
                        ...privateMessages,
                        [activeChat.name]: [...messages, newMessage],
                      });
                    } else {
                      setRoomMessages({
                        ...roomMessages,
                        [currentRoom]: [...messages, newMessage],
                      });
                    }

                    setMessageText("");
                    setNotification(`Message sent to ${activeChat.name}`);
                  };
                }
              }}
            />

            <button onClick={sendMessage}>Send</button>
          </div>
        </section>

        {/* Online Users */}
        <aside className="users-sidebar">
          <h2>Online Users</h2>

          {roomUsers[currentRoom].map((username) => (
            <div className="user" key={username}>
              <span className="status"></span>
              {username}
            </div>
          ))}
        </aside>
      </main>
    </div>
  );
}

export default App;
