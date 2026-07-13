#!/bin/sh
# Generates web-app/config.js from environment variables at build time.
# Real values live in Netlify's Environment Variables settings, never in git.

cat > web-app/config.js << EOF
window.FIREBASE_CONFIG = {
  apiKey: "${FIREBASE_API_KEY}",
  databaseURL: "${FIREBASE_DB_URL}"
};
EOF

echo "config.js generated from environment variables."
