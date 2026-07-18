# ParkSense 🚗🅿️  
### IoT-Based Smart Parking System using ESP32 & Firebase

ParkSense is a smart parking solution that shows real-time parking slot availability on a live map, with in-app turn-by-turn navigation and a real ESP32 hardware companion.

🔗 **Live demo**: _add your Netlify URL here, e.g. https://park-sense-official.netlify.app_

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

## 🔩 Hardware Needed
- ESP32
- 5x push buttons (no external resistor needed — code uses `INPUT_PULLUP`)
- 5x LEDs + up to 5x 220Ω resistors (fewer is fine — just wire however many you have; unwired pins are simply inert)
- Breadboard + jumper wires
- **No OLED needed** — the web app already shows richer live status than a small display could.

LEDs are ON while their slot is free, and turn OFF once marked occupied.

## 🗺️ Features
- Real-time slot updates across multiple parking areas, shown on a live map
- Custom pin markers, with the nearest available area highlighted and pulsing
- In-app turn-by-turn navigation with voice guidance, live distance countdown, alternate routes, and a live speed-based ETA — no Google API
- Live destination status — availability keeps updating even after you've started navigating there
- Search bar (parking areas + general places) and a nearby-areas list
- Floating feedback form, written straight to Firebase
- ESP32 hardware companion (buttons + LEDs) that writes real slot state to the same live database

## 🧪 Simulation
- ESP32 simulated using Wokwi
- Firebase REST API used due to SSL limitations in simulation

## 📸 Screenshots
See `assets/screenshots/` folder.

## 🔐 Setting Up a Fresh Deployment
If you're cloning this repo to run your own instance (your own Firebase project, your own ESP32), here's the full setup:

1. **Publish the rules** — copy `firebase-rules.json` from this repo into your Firebase Console → Realtime Database → Rules → paste → Publish. These rules:
   - Keep reads public (fine — parking availability isn't sensitive)
   - Require auth for any parking-data write
   - Validate that `status` is only ever `"free"`/`"occupied"` and `type` only `"bike"`/`"car"`/`"van"` — so even a leaked secret can't be used to write garbage data
   - Block writes to `lat`/`lng`/`name` or wholesale slot replacement — only the `status` field is writable
   - Allow public, validated writes to `feedback` — but keep it unreadable to visitors

2. **Get a database secret**: Firebase Console → Project Settings → Service Accounts → "Database secrets" tab.
   - If that tab exists (older projects): copy the secret.
   - If it's not there (newer projects don't expose legacy secrets): you'd need anonymous Firebase Auth + ID token refresh instead of the current legacy-secret approach.

3. **Firmware** (`firmware/`): copy `secrets.example.h` → `secrets.h`, fill in WiFi credentials + your database secret.

4. **Web app** (`web-app/`): copy `config.example.js` → `config.js`, fill in your Firebase `apiKey` and `databaseURL`. (For a Netlify deploy, use environment variables instead — see below.)

## 🚀 Live Speed & Dynamic ETA
While a route is active, ParkSense tracks your real driving speed (GPS-reported where available, calculated from movement otherwise) and shows a live "at your current speed: ~X min left" estimate — recalculated continuously as you actually drive, not just a one-time static guess. Shown both as a HUD-style speed badge and inline in the navigation banner.

## 🎨 Design
Visual identity is built around real road-signage and car-dashboard cues rather than a generic template: `Rajdhani` (technical, signage-like) for branding and headers, `Inter` for readable body text, and `IBM Plex Mono` for all live numeric readouts (speed, ETA, distance) — giving live data a genuine "dashboard" feel. Deep navy + signal amber form the brand palette, kept separate from the green/red slot-status colors so availability stays instantly readable.

Custom teardrop pin markers (not the default Google Maps dot icons) mark each parking area, with the recommended pick pulsing gently to draw the eye. Popups show occupancy as progress bars per vehicle type instead of flat text badges. A loading screen covers the initial data fetch instead of showing a blank map.

Turn-by-turn instructions are built from the raw OSRM maneuver type (left/right/straight/roundabout/etc.) rather than the library's default phrasing, since OSRM's first instruction is normally a compass direction ("Head west") — not how a driver actually wants directions given.

## 🧭 Directions & Live Navigation
Tapping "Go" on a parking area draws the route directly on ParkSense's own map — no Google API, no key, no billing. Built entirely on free tools:

- **Routing**: [OSRM](http://project-osrm.org/)'s free public routing server
- **Voice guidance**: the browser's built-in Web Speech API (`SpeechSynthesisUtterance`) — no external TTS service
- **Live tracking**: browser Geolocation `watchPosition`

**Features:**
- Distance + drive-time estimate, plus a separate ETA breakdown per vehicle type (🏍️ bike, 🚗 car, 🚐 van) with the assumed average speed shown alongside each — computed from route distance and assumed speeds (25/35/30 km/h respectively — edit `VEHICLE_SPEEDS` in `index.html` if you have better numbers for your area). Durations auto-format as minutes, hours, or days depending on scale.
- Alternate routes are drawn as dashed gray lines — click one to make it the active route
- **"▶ Start Navigation"** begins live turn-by-turn guidance: the map zooms in and follows your live position, a banner shows a live "in X m" countdown to the next turn plus the instruction itself, spoken aloud as you approach, with basic automatic re-routing if you drift off the route
- A 🎯 recenter button snaps the map back to show both your location and the parking areas together

**Known limitations (be upfront about these if presenting this project):**
- OSRM's free public server has **no real-time traffic data** — ETAs are static distance/speed estimates, not traffic-aware like Google Maps. It's also a shared community server, not guaranteed uptime under load — fine for demos, not for production traffic at scale.
- Voice quality depends on the browser/OS's built-in text-to-speech engine, not a polished dedicated voice service.
- Re-routing on drift is a simple "recalculate from current position" — not as smooth or predictive as commercial nav apps.

## 🔍 Search & Discovery
- **Search bar** (top of screen): checks parking area names first, then falls back to [OpenStreetMap's free Nominatim geocoder](https://nominatim.org/) for general place search — no Google API, no key. Click a result to fly there.
- **📋 Nearby areas panel**: lists every known parking area with live distance and free-slot count, sorted nearest-first — solves the problem of parking markers being off-screen (e.g. testing from far outside the demo city) without needing to manually zoom/pan to find them.

## 🅿️ Live Destination Status
Once you pick a destination (via "Go" or during active navigation), its name and live per-vehicle availability stay visible continuously in the route panel and nav banner — refreshed on every Firebase update. If someone else takes the last spot while you're still driving there, you'll see it change in real time, not just at the moment you clicked.

## 💬 Feedback
A floating feedback button opens a small form that writes directly to Firebase (`feedback` node) — no backend needed. Entries are write-only from the public site (`.read: false` in the rules) so visitors can't browse each other's submissions; you can still read them yourself anytime via the Firebase Console's Data tab, since rules don't restrict console/owner access. Validated server-side (non-empty, max 500 characters) so it can't be abused for arbitrary data injection.

**Requires a rules update** — the `feedback` node needs its own permission block, separate from the parking-slot rules. Re-publish `firebase-rules.json` (it now includes both) to Firebase Console → Realtime Database → Rules, or feedback submissions will fail.

## 🔒 Security
Realistic security posture for a static site + Firebase backend — no server of our own to harden, so this focuses on what's actually within reach:

**What's done:**
- **Database rules** (`firebase-rules.json`) — parking data: public read, auth-required write, server-side validated (only `status`/`type` fields, only allowed values). Feedback: public write, but validated (non-empty, ≤500 chars) and **not publicly readable**, so visitors can't browse each other's submissions.
- **XSS hardening** — any text that isn't a fixed string we wrote (area names, search results from Nominatim) is HTML-escaped before insertion, and the "Go"/area-list buttons pass only a safe Firebase key through onclick handlers rather than interpolating free text into HTML attributes.
- **No secrets in the repo** — Firebase config and ESP32 credentials are gitignored and injected via Netlify environment variables at build time (see below).
- **Light client-side throttling** on search and feedback submission — prevents accidental spam/abuse of the free services, though this is a UX courtesy, not a real security boundary (see below).

**What's honestly out of reach for a static frontend:**
- **True rate-limiting / DDoS protection** requires server-side enforcement. A determined attacker can call the Firebase REST API directly, bypassing the website entirely — client-side throttling only stops accidental double-clicks, not deliberate abuse. If this ever needs to be bulletproof: enable **Firebase App Check** (verifies requests actually come from your app, configured in the Firebase Console — a few clicks, no code change needed here) and consider Cloud Functions for anything requiring real rate-limiting.
- **Netlify** already sits behind a CDN with baseline bot/DDoS mitigation by default — nothing to configure, but also nothing this repo controls directly.
- **OSRM and Nominatim** (the free routing/search services) have no uptime guarantee and aren't meant for high production traffic — fine for a demo/portfolio project, a real concern only at scale.

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

## 👤 Authors
**Haashim Aslam** and **M.T. Abdul Khalid**
