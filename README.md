# ParkSense 🚗🅿️  
### IoT-Based Smart Parking System using ESP32 & Firebase

ParkSense is a smart parking solution that shows real-time parking slot availability on a live map.

## 🔧 Technologies Used
- ESP32 (simulated in Wokwi)
- Firebase Realtime Database
- Firebase REST API
- Leaflet.js (Maps)
- HTML, JavaScript

## ⚙️ How It Works
1. ESP32 monitors parking slots using buttons (simulation).
2. Slot status is sent to Firebase using REST API.
3. A web application reads Firebase data in real time.
4. Parking areas are shown on a map with live slot availability.

## 🗺️ Features
- Real-time slot updates
- Multiple parking areas
- Live map visualization
- Cloud-based backend

## 🧪 Simulation
- ESP32 simulated using Wokwi
- Firebase REST API used due to SSL limitations in simulation

## 📸 Screenshots
See `assets/screenshots/` folder.

## 🔐 Going Live, Securely
The live database currently has `.write: false` — nobody, including the ESP32, can write yet. To make it live while keeping it locked down:

1. **Publish the tightened rules** — copy `firebase-rules.json` from this repo into Firebase Console → Realtime Database → Rules → paste → Publish. These rules:
   - Keep reads public (fine — parking availability isn't sensitive)
   - Require auth for any write
   - Validate that `status` is only ever `"free"`/`"occupied"` and `type` only `"bike"`/`"car"`/`"van"` — so even a leaked secret can't be used to write garbage data
   - Block writes to `lat`/`lng`/`name` or wholesale slot replacement — only the `status` field is writable

2. **Get a database secret**: Firebase Console → Project Settings → Service Accounts → "Database secrets" tab.
   - If that tab exists (older projects): copy the secret.
   - If it's not there (newer projects don't expose legacy secrets): you'd need anonymous Firebase Auth + ID token refresh instead — let me know if you hit this and I'll wire that up instead, it's a bigger firmware change.

3. **Firmware** (`firmware/`): copy `secrets.example.h` → `secrets.h`, fill in WiFi credentials + the database secret.

4. **Web app** (`web-app/`): copy `config.example.js` → `config.js`, fill in your Firebase `apiKey` and `databaseURL`.

## 🧭 Directions
Tapping "Go" on a parking area draws a live route on the map itself (distance + ETA shown top-right), using real browser geolocation as the start point and [OSRM's free public routing service](http://project-osrm.org/) — no API key or billing needed.

**Known limitation**: OSRM's public demo server isn't guaranteed uptime/rate limits, since it's a free community service, not meant for production traffic. Fine for a portfolio/demo project; if this needs to be rock-solid for real users later, swap in a paid routing API (Mapbox, Google Directions, ORS) — the `startRoute()` function is the only place that would need to change.

## 🚀 Deploy to Netlify (public URL, auto-updates on every push)

1. Push this repo to GitHub (see repo root instructions, or your own workflow).
2. Go to [netlify.com](https://netlify.com) → **Add new site** → **Import an existing project** → connect GitHub → select this repo.
3. Netlify will auto-detect `netlify.toml` in this repo, which already sets:
   - Build command: generates `web-app/config.js` at build time
   - Publish directory: `web-app`
   You shouldn't need to change these — just confirm and continue.
4. Before the first deploy, go to **Site settings → Environment variables** and add:
   - `FIREBASE_API_KEY` → your Firebase apiKey
   - `FIREBASE_DB_URL` → your Firebase databaseURL
   (Your real key is never committed to git — Netlify injects it at build time only.)
5. Click **Deploy site**. You'll get a public `https://<random-name>.netlify.app` URL (renameable in Site settings → Domain management).
6. From now on, every `git push` to this repo auto-triggers a new Netlify build+deploy — no manual re-upload needed.

⚠️ Note: your Firebase Database security rules (`firebase-rules.json`) are the real access-control boundary — the `apiKey` alone doesn't restrict who can read/write, so make sure those rules are published before/alongside going live publicly.

## 👤 Author
**Haashim Aslam**
