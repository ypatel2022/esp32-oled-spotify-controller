import * as dotenv from 'dotenv'
dotenv.config()

async function getAccessToken() {
  const refresh_token = process.env.SPOTIFY_REFRESH_TOKEN || ''

  const response = await fetch('https://accounts.spotify.com/api/token', {
    method: 'POST',
    headers: {
      Authorization: `Basic ${Buffer.from(
        `${process.env.SPOTIFY_CLIENT_ID}:${process.env.SPOTIFY_CLIENT_SECRET}`
      ).toString('base64')}`,
      'Content-Type': 'application/x-www-form-urlencoded',
    },
    body: new URLSearchParams({
      grant_type: 'refresh_token',
      refresh_token,
    }),
  })

  return response.json()
}

export async function currentPlaybackState() {
  const { access_token } = await getAccessToken()

  const response = await fetch('https://api.spotify.com/v1/me/player', {
    headers: {
      Authorization: `Bearer ${access_token}`,
    },
  })

  return response.json()
}

export async function togglePlayback() {
  const { access_token } = await getAccessToken()

  const currentPlaybackStateData = await currentPlaybackState()

  if (currentPlaybackStateData.is_playing) {
    return fetch('https://api.spotify.com/v1/me/player/pause', {
      method: 'PUT',
      headers: {
        Authorization: `Bearer ${access_token}`,
      },
    })
  } else {
    return fetch('https://api.spotify.com/v1/me/player/play', {
      method: 'PUT',
      headers: {
        Authorization: `Bearer ${access_token}`,
      },
    })
  }
}

export async function skipToNext() {
  const { access_token } = await getAccessToken()

  const response = await fetch('https://api.spotify.com/v1/me/player/next', {
    method: 'POST',

    headers: {
      Authorization: `Bearer ${access_token}`,
    },
  })

  return response.json()
}

export async function skipToPrevious() {
  const { access_token } = await getAccessToken()

  const response = await fetch(
    'https://api.spotify.com/v1/me/player/previous',
    {
      method: 'POST',

      headers: {
        Authorization: `Bearer ${access_token}`,
      },
    }
  )

  return response.json()
}

export async function getQueue() {
  const { access_token } = await getAccessToken()

  const response = await fetch('https://api.spotify.com/v1/me/player/queue', {
    headers: {
      Authorization: `Bearer ${access_token}`,
    },
  })

  return response.json()
}
