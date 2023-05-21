import * as dotenv from 'dotenv'
dotenv.config()
import express from 'express'

const app = express()
const port = 8000

import { currentlyPlaying, currentPlaybackState } from './lib/spotify.js'

app.get('/is-playing', async (req, res) => {
  try {
    const currentPlaybackStateData = await currentPlaybackState()

    res.json(currentPlaybackStateData.is_playing)
  } catch {
    res.json(false)
  }
})

app.get('/currently-playing', async (req, res) => {
  await currentlyPlaying()
    .then((response) => response.json())
    .then((data) => {
      const filteredData = {
        album: data.item.album.name,
        artist: data.item.artists[0].name,
        song: data.item.name,
        timestamp: data.timestamp,
        progress: data.progress_ms,
      }

      res.json(filteredData)
    })
    .catch((error) => {
      console.log(error)
      res.json(error)
    })

  res.json()
})

app.get('/get-lyrics', async (req, res) => {
  const currentPlaybackStateData = await currentPlaybackState()

  const trackId = currentPlaybackStateData.item.id

  const url = `https://spotify-lyric-api.herokuapp.com/?trackid=${trackId}`

  const response = await fetch(url).then((response) => response.json())

  res.json(response)
})

app.listen(port, () => {
  console.log(`Server is running on http://localhost:${port}`)
})
