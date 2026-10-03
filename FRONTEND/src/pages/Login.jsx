import { useState } from 'react'



function Login({ onLogin, onRegister }) {
  const [username, setUsername] = useState('')
  return (
    <div className="login-page">

      <div className="login-card">

        <h1>Linux Chat</h1>

        <p className="login-subtitle">
          Sign in to continue chatting
        </p>

        <form
          onSubmit={(event) => {
            event.preventDefault()
            onLogin(username)
          }}
        >

          <div className="form-group">
            <label>Username</label>

            <input
  type="text"
  placeholder="Enter username"
  value={username}
  onChange={(event) => setUsername(event.target.value)}
  required
/>
          </div>

          <div className="form-group">
            <label>Password</label>

            <input
              type="password"
              placeholder="Enter password"
              required
            />
          </div>

          <button type="submit" className="login-button">
            Login
          </button>

        </form>

        <p className="register-text">
  Don't have an account?{' '}
  <span onClick={onRegister}>
    Register
  </span>
</p>

      </div>

    </div>
  )
}

export default Login