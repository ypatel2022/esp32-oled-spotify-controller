import * as dotenv from 'dotenv'
dotenv.config()
import express from 'express'

const app = express()
const port = 8000

import {
  currentPlaybackState,
  togglePlayback,
  skipToNext,
  skipToPrevious,
  getQueue,
} from './lib/spotify.js'
import { millisecondsToMinutes } from './lib/utils.js'

app.get('/current-playback-state', async (req, res) => {
  try {
    const data = await currentPlaybackState()
    const queue = await getQueue()
    const nextSong = queue.queue[0].name
    const nextArtist = queue.queue[0].artists[0].name

    const isSingle = data.item.album.album_type === 'single'

    const filteredData = {
      album: isSingle ? 'Single' : data.item.album.name,
      artists: data.item.artists[0].name,
      song: data.item.name,
      progress: millisecondsToMinutes(data.progress_ms),
      duration: millisecondsToMinutes(data.item.duration_ms),
      nextSong: nextSong,
      nextArtist: nextArtist,
    }

    res.json(filteredData)
  } catch {
    res.json('error')
  }
})

app.get('/toggle-playback', async (req, res) => {
  try {
    await togglePlayback()

    res.json(true)
  } catch {
    res.json(false)
  }
})

app.get('/skip-to-next', async (req, res) => {
  try {
    await skipToNext()

    res.json(true)
  } catch {
    res.json(false)
  }
})

app.get('/skip-to-previous', async (req, res) => {
  try {
    await skipToPrevious()

    res.json(true)
  } catch {
    res.json(false)
  }
})

app.listen(port, () => {
  console.log(`Server is running on http://localhost:${port}`)
})
