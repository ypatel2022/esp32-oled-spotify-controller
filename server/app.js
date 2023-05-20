import * as dotenv from 'dotenv'
dotenv.config()
import express from 'express'

const app = express()
const port = 8000

import { currentlyPlaying } from './lib/spotify.js'

app.get('/currently-playing', async (req, res) => {
  await currentlyPlaying()
    .then((response) => response.json())
    .then((data) => {
      res.json(data)
    })
    .catch((error) => {
      console.log(error)
      res.json(error)
    })
})

app.listen(port, () => {
  console.log(`Server is running on http://localhost:${port}`)
})
