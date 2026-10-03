import { useState } from 'react'

function Register({ onRegister, onBackToLogin }) {
  const [username, setUsername] = useState('')
  const [password, setPassword] = useState('')
  const [confirmPassword, setConfirmPassword] = useState('')

  const handleSubmit = (event) => {
    event.preventDefault()

    if (password !== confirmPassword) {
      alert('Passwords do not match')
      return
    }

    onRegister(username)
  }

  return (
    <div className="login-page">

      <div className="login-card">

        <h1>Create Account</h1>

        <p className="login-subtitle">
          Register for Linux Chat
        </p>

        <form onSubmit={handleSubmit}>

          <div className="form-group">
            <label>Username</label>

            <input
              type="text"
              placeholder="Choose a username"
              value={username}
              onChange={(event) => setUsername(event.target.value)}
              required
            />
          </div>

          <div className="form-group">
            <label>Password</label>

            <input
              type="password"
              placeholder="Create a password"
              value={password}
              onChange={(event) => setPassword(event.target.value)}
              required
            />
          </div>

          <div className="form-group">
            <label>Confirm Password</label>

            <input
              type="password"
              placeholder="Confirm your password"
              value={confirmPassword}
              onChange={(event) => setConfirmPassword(event.target.value)}
              required
            />
          </div>

          <button type="submit" className="login-button">
            Register
          </button>

        </form>

        <p className="register-text">
          Already have an account?{' '}
          <span onClick={onBackToLogin}>
            Login
          </span>
        </p>

      </div>

    </div>
  )
}

export default Register