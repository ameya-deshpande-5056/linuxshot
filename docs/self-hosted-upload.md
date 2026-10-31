# Self-Hosted Private Upload Endpoint Guide

Set up your own secure, private screenshot upload server so your LinuxShot captures never touch a third-party service.

> [!NOTE]
> LinuxShot's built-in **Self-Hosted Provider** uploads images as a standard HTTP `multipart/form-data` POST with
> a single part named `image`. Any server that accepts that format and replies with a JSON body
> containing a `url`, `link`, or `data.url` field (or a plain-text URL) will work out of the box — no
> client-side changes needed.

---

## Table of Contents

1. [Why Self-Host?](#1-why-self-host)
2. [How LinuxShot Uploads Work](#2-how-linuxshot-uploads-work)
3. [Option A — Minimal Python Server (Flask)](#3-option-a--minimal-python-server-flask)
4. [Option B — Minimal Go Server](#4-option-b--minimal-go-server)
5. [Option C — Minimal Node.js Server (Express)](#5-option-c--minimal-nodejs-server-express)
6. [Adding HTTPS with Let's Encrypt (Certbot + Nginx)](#6-adding-https-with-lets-encrypt-certbot--nginx)
7. [Authentication & Access Control](#7-authentication--access-control)
8. [Hardening Checklist](#8-hardening-checklist)
9. [Connecting LinuxShot to Your Endpoint](#9-connecting-linuxshot-to-your-endpoint)
10. [FAQ & Troubleshooting](#10-faq--troubleshooting)

---

## 1. Why Self-Host?

The default **Prntscr** provider uploads screenshots to `upload.prntscr.com` over HTTPS, but:

- Images are **publicly accessible** — anyone with the generated URL can view them.
- You have **no control** over retention, deletion, or data-processing policies.
- Screenshots of code, credentials, internal dashboards, or personal content carry **privacy and security risk**.

A private endpoint gives you:

| Benefit | Detail |
|---|---|
| **Full data ownership** | Images stay on your hardware or your VPS |
| **Access control** | Add API-key, token, or IP-based authentication |
| **TLS everywhere** | End-to-end encryption between LinuxShot and your server |
| **Retention policies** | Auto-purge images after *N* days with a simple cron job |
| **Audit trail** | Log every upload with timestamp and source IP |

---

## 2. How LinuxShot Uploads Work

When you press the **Upload** button (or `Ctrl+D`), LinuxShot's `SelfHostedProvider` does the following:

1. Encodes the captured (and annotated) `QImage` into the selected format (PNG or JPEG).
2. Builds an HTTP **`multipart/form-data`** POST request with:
   - A part named **`image`** containing the raw image bytes.
   - `Content-Type` set to `image/png` or `image/jpeg`.
   - A filename of `screenshot.png` (or `.jpg`).
3. Sends the request to the URL configured in **Preferences → Upload & Network → Upload Endpoint**.
4. Parses the response — any of the following formats are accepted:

```jsonc
// Format 1 — top-level "url" or "link"
{ "url": "https://your-server.example.com/i/abc123.png" }

// Format 2 — nested under "data"
{ "data": { "url": "https://your-server.example.com/i/abc123.png" } }
```

```text
# Format 3 — plain-text URL
https://your-server.example.com/i/abc123.png
```

5. Copies the returned URL to the clipboard (if *Auto-copy link* is enabled).

> [!IMPORTANT]
> Your server **must** respond with one of the three formats above. A `200 OK` without a parsable URL
> will be treated as an error by LinuxShot.

---

## 3. Option A — Minimal Python Server (Flask)

A lightweight reference server in ~40 lines of Python.

### Prerequisites

```bash
pip install flask
```

### Server Code — `upload_server.py`

```python
#!/usr/bin/env python3
"""Minimal private upload endpoint for LinuxShot."""

import os
import uuid
from flask import Flask, request, jsonify, send_from_directory, abort

app = Flask(__name__)

UPLOAD_DIR = os.environ.get("UPLOAD_DIR", "./uploads")
BASE_URL   = os.environ.get("BASE_URL", "http://localhost:9100")  # Public-facing URL
API_KEY    = os.environ.get("API_KEY", "")                        # Set for auth; empty = open

os.makedirs(UPLOAD_DIR, exist_ok=True)

ALLOWED_TYPES = {"image/png", "image/jpeg", "image/bmp"}
MAX_SIZE_MB   = 25


@app.before_request
def check_auth():
    if API_KEY and request.endpoint == "upload_image":
        token = request.headers.get("Authorization", "").removeprefix("Bearer ").strip()
        if token != API_KEY:
            abort(401, description="Invalid or missing API key.")


@app.route("/upload", methods=["POST"])
def upload_image():
    if "image" not in request.files:
        return jsonify({"error": "No 'image' part in request"}), 400

    img = request.files["image"]
    if img.content_type not in ALLOWED_TYPES:
        return jsonify({"error": f"Unsupported content type: {img.content_type}"}), 415

    # Read into memory to enforce size limit
    data = img.read()
    if len(data) > MAX_SIZE_MB * 1024 * 1024:
        return jsonify({"error": f"File exceeds {MAX_SIZE_MB} MB limit"}), 413

    ext = "png" if "png" in img.content_type else ("jpg" if "jpeg" in img.content_type else "bmp")
    filename = f"{uuid.uuid4().hex}.{ext}"
    filepath = os.path.join(UPLOAD_DIR, filename)

    with open(filepath, "wb") as f:
        f.write(data)

    return jsonify({"url": f"{BASE_URL}/i/{filename}"}), 200


@app.route("/i/<filename>")
def serve_image(filename):
    return send_from_directory(UPLOAD_DIR, filename)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=9100)
```

### Running

```bash
# No authentication (local testing)
python3 upload_server.py

# With authentication
API_KEY="my-secret-key-1234" BASE_URL="https://screenshots.example.com" python3 upload_server.py
```

Test it with `curl`:

```bash
curl -X POST http://localhost:9100/upload \
  -H "Authorization: Bearer my-secret-key-1234" \
  -F "image=@/tmp/test.png;type=image/png"
# => {"url": "http://localhost:9100/i/a1b2c3d4.png"}
```

---

## 4. Option B — Minimal Go Server

A single-binary, zero-dependency Go server for production-grade performance.

### `main.go`

```go
package main

import (
	"crypto/rand"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

var (
	uploadDir = envOr("UPLOAD_DIR", "./uploads")
	baseURL   = envOr("BASE_URL", "http://localhost:9100")
	apiKey    = envOr("API_KEY", "")
	maxBytes  int64 = 25 << 20 // 25 MB
)

func envOr(key, fallback string) string {
	if v := os.Getenv(key); v != "" {
		return v
	}
	return fallback
}

func randomHex(n int) string {
	b := make([]byte, n)
	rand.Read(b)
	return hex.EncodeToString(b)
}

func authMiddleware(next http.HandlerFunc) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		if apiKey != "" {
			token := strings.TrimPrefix(r.Header.Get("Authorization"), "Bearer ")
			token = strings.TrimSpace(token)
			if token != apiKey {
				http.Error(w, `{"error":"unauthorized"}`, http.StatusUnauthorized)
				return
			}
		}
		next(w, r)
	}
}

func handleUpload(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, `{"error":"method not allowed"}`, http.StatusMethodNotAllowed)
		return
	}
	r.Body = http.MaxBytesReader(w, r.Body, maxBytes)

	file, header, err := r.FormFile("image")
	if err != nil {
		http.Error(w, `{"error":"missing 'image' part"}`, http.StatusBadRequest)
		return
	}
	defer file.Close()

	ct := header.Header.Get("Content-Type")
	ext := "png"
	switch {
	case strings.Contains(ct, "jpeg"):
		ext = "jpg"
	case strings.Contains(ct, "bmp"):
		ext = "bmp"
	}

	filename := fmt.Sprintf("%s.%s", randomHex(16), ext)
	dst, err := os.Create(filepath.Join(uploadDir, filename))
	if err != nil {
		http.Error(w, `{"error":"storage error"}`, http.StatusInternalServerError)
		return
	}
	defer dst.Close()
	io.Copy(dst, file)

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(map[string]string{
		"url": fmt.Sprintf("%s/i/%s", baseURL, filename),
	})
}

func main() {
	os.MkdirAll(uploadDir, 0750)
	http.HandleFunc("/upload", authMiddleware(handleUpload))
	http.Handle("/i/", http.StripPrefix("/i/", http.FileServer(http.Dir(uploadDir))))
	log.Printf("Listening on :9100  (UPLOAD_DIR=%s)", uploadDir)
	log.Fatal(http.ListenAndServe(":9100", nil))
}
```

### Build & Run

```bash
go build -o linuxshot-upload main.go
API_KEY="supersecret" BASE_URL="https://shots.example.com" ./linuxshot-upload
```

---

## 5. Option C — Minimal Node.js Server (Express)

### Prerequisites

```bash
mkdir linuxshot-upload && cd linuxshot-upload
npm init -y
npm install express multer uuid
```

### `server.js`

```javascript
const express = require("express");
const multer  = require("multer");
const { v4: uuidv4 } = require("uuid");
const path    = require("path");
const fs      = require("fs");

const app       = express();
const PORT      = process.env.PORT || 9100;
const UPLOAD_DIR = process.env.UPLOAD_DIR || "./uploads";
const BASE_URL  = process.env.BASE_URL || `http://localhost:${PORT}`;
const API_KEY   = process.env.API_KEY || "";

fs.mkdirSync(UPLOAD_DIR, { recursive: true });

const ALLOWED = new Set(["image/png", "image/jpeg", "image/bmp"]);
const upload  = multer({
  limits: { fileSize: 25 * 1024 * 1024 },
  fileFilter: (_req, file, cb) => cb(null, ALLOWED.has(file.mimetype)),
});

// Auth middleware
app.use("/upload", (req, res, next) => {
  if (API_KEY) {
    const token = (req.headers.authorization || "").replace(/^Bearer\s+/i, "").trim();
    if (token !== API_KEY) return res.status(401).json({ error: "unauthorized" });
  }
  next();
});

app.post("/upload", upload.single("image"), (req, res) => {
  if (!req.file) return res.status(400).json({ error: "No 'image' part in request" });

  const ext = req.file.mimetype.includes("jpeg") ? "jpg"
            : req.file.mimetype.includes("bmp")  ? "bmp" : "png";
  const filename = `${uuidv4().replace(/-/g, "")}.${ext}`;
  const dest = path.join(UPLOAD_DIR, filename);

  fs.writeFileSync(dest, req.file.buffer);
  res.json({ url: `${BASE_URL}/i/${filename}` });
});

app.use("/i", express.static(UPLOAD_DIR));

app.listen(PORT, "0.0.0.0", () => {
  console.log(`Upload server listening on :${PORT}`);
});
```

### Running

```bash
API_KEY="my-key" BASE_URL="https://shots.example.com" node server.js
```

---

## 6. Adding HTTPS with Let's Encrypt (Certbot + Nginx)

> [!CAUTION]
> Running any of the servers above over plain HTTP means your screenshots travel
> **unencrypted** across the network. Always terminate TLS in front of the upload endpoint.

### 6.1 Install Nginx and Certbot

```bash
# Debian / Ubuntu
sudo apt update && sudo apt install -y nginx certbot python3-certbot-nginx
```

### 6.2 Create an Nginx Site

Create `/etc/nginx/sites-available/linuxshot-upload`:

```nginx
server {
    listen 80;
    server_name screenshots.example.com;   # ← replace with your domain

    # Certbot will rewrite this block to redirect to HTTPS
    location / {
        return 301 https://$host$request_uri;
    }
}

server {
    listen 443 ssl http2;
    server_name screenshots.example.com;

    # Certbot will insert certificate paths here
    # ssl_certificate     /etc/letsencrypt/live/screenshots.example.com/fullchain.pem;
    # ssl_certificate_key /etc/letsencrypt/live/screenshots.example.com/privkey.pem;

    client_max_body_size 25M;

    # Proxy /upload to the local upload server
    location /upload {
        proxy_pass         http://127.0.0.1:9100;
        proxy_set_header   Host $host;
        proxy_set_header   X-Real-IP $remote_addr;
        proxy_set_header   X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header   X-Forwarded-Proto $scheme;
    }

    # Serve uploaded images directly via Nginx (faster than proxying)
    location /i/ {
        alias /srv/linuxshot-uploads/;   # ← match your UPLOAD_DIR
        expires 30d;
        add_header Cache-Control "public, immutable";
    }
}
```

Enable and test:

```bash
sudo ln -s /etc/nginx/sites-available/linuxshot-upload /etc/nginx/sites-enabled/
sudo nginx -t && sudo systemctl reload nginx
```

### 6.3 Obtain a TLS Certificate

```bash
sudo certbot --nginx -d screenshots.example.com
```

Certbot will automatically modify your Nginx config to add the certificate paths and set up auto-renewal.

Verify renewal is scheduled:

```bash
sudo certbot renew --dry-run
```

### 6.4 Alternative — Caddy (Automatic HTTPS)

If you prefer zero-config TLS, [Caddy](https://caddyserver.com) obtains and renews certificates automatically:

```
# /etc/caddy/Caddyfile
screenshots.example.com {
    reverse_proxy /upload 127.0.0.1:9100
    file_server /i/* {
        root /srv/linuxshot-uploads
    }
}
```

```bash
sudo systemctl restart caddy
```

---

## 7. Authentication & Access Control

### 7.1 Bearer Token (API Key)

All three server examples above support `API_KEY` authentication out of the box. When set, LinuxShot
must send an `Authorization: Bearer <key>` header.

> [!NOTE]
> LinuxShot's current `SelfHostedProvider` does **not** send an `Authorization` header by default.
> To use token-based auth today, you can enforce authentication at the Nginx layer instead
> (see §7.2 below), or contribute a patch to add a configurable auth header to `SelfHostedProvider`.

### 7.2 Nginx-Level Basic Auth (Works Without Client Changes)

This approach requires no modifications to LinuxShot — the browser-style auth prompt is handled at the reverse-proxy layer by matching a secret path segment.

However, a simpler method is to use an **unpublished secret URL path** as a lightweight auth token:

```nginx
# Only forward requests whose path begins with a secret prefix
location /upload/s3cr3t-t0ken-here {
    rewrite ^/upload/s3cr3t-t0ken-here(.*) /upload$1 break;
    proxy_pass http://127.0.0.1:9100;
}

# Block direct access to /upload
location = /upload {
    return 403;
}
```

Then set LinuxShot's endpoint to:
```
https://screenshots.example.com/upload/s3cr3t-t0ken-here
```

### 7.3 IP Allowlisting via Firewall

Restrict access to your upload server to only known IP addresses:

```bash
# UFW example — allow only your home/office IP
sudo ufw default deny incoming
sudo ufw allow from 203.0.113.42 to any port 443 proto tcp
sudo ufw enable
```

Or via Nginx:

```nginx
location /upload {
    allow 203.0.113.42;  # your home IP
    allow 10.0.0.0/8;    # your VPN range
    deny all;

    proxy_pass http://127.0.0.1:9100;
}
```

---

## 8. Hardening Checklist

Use this checklist before exposing your upload server to the internet:

- [ ] **TLS only** — redirect all HTTP to HTTPS; disable plain HTTP upload.
- [ ] **Authentication** — use at least one of: API key, secret URL path, IP allowlist, or VPN.
- [ ] **File-type validation** — only accept `image/png`, `image/jpeg`, `image/bmp` (all examples above enforce this).
- [ ] **File-size limit** — cap at 25 MB or lower (prevents abuse).
- [ ] **Randomized filenames** — use UUIDs, never user-supplied filenames (prevents path traversal attacks).
- [ ] **Serve uploads from a separate domain/subdomain** — prevents cookie theft if your main site sets cookies.
- [ ] **Set security headers** on served images:
  ```nginx
  add_header X-Content-Type-Options "nosniff" always;
  add_header Content-Security-Policy "default-src 'none'" always;
  ```
- [ ] **Rate limiting** — add `limit_req` in Nginx to prevent upload flooding:
  ```nginx
  limit_req_zone $binary_remote_addr zone=uploads:10m rate=5r/m;
  location /upload {
      limit_req zone=uploads burst=3 nodelay;
      proxy_pass http://127.0.0.1:9100;
  }
  ```
- [ ] **Log rotation** — use `logrotate` for both Nginx and application logs.
- [ ] **Automated cleanup** — add a cron job to purge images older than *N* days:
  ```bash
  # Delete uploads older than 30 days, runs daily at 03:00
  0 3 * * * find /srv/linuxshot-uploads -type f -mtime +30 -delete
  ```
- [ ] **Firewall** — close all ports except 22 (SSH), 80, and 443.
- [ ] **Automatic updates** — enable `unattended-upgrades` on your server.

---

## 9. Connecting LinuxShot to Your Endpoint

Once your server is running and reachable:

1. Open LinuxShot **Preferences** (right-click tray icon → **Preferences...**).
2. Go to the **Upload & Network** tab.
3. In the **Upload Endpoint** field, enter your server's upload URL:
   ```
   https://screenshots.example.com/upload
   ```
   Or, if using a secret-path token (§7.2):
   ```
   https://screenshots.example.com/upload/s3cr3t-t0ken-here
   ```
4. **Uncheck** the *"Use prntscr.com"* checkbox to ensure LinuxShot uses your private endpoint instead of the public Prntscr service.
5. Optionally enable **Auto-copy link** to have the returned URL copied to your clipboard automatically.
6. Click **OK**.

### Verifying the Connection

1. Take any screenshot with LinuxShot (`Print Screen` → select region).
2. Click the **Upload** button (☁) on the action toolbar, or press `Ctrl+D`.
3. If successful, you'll see a desktop notification with the image URL. The URL should point to your domain.
4. Open the URL in a browser to confirm the image is served correctly over HTTPS.

---

## 10. FAQ & Troubleshooting

### "Unrecognized response format from server"

Your server returned a response that LinuxShot couldn't parse. Ensure the response body is:
- A JSON object with a `url` or `link` key at the top level, **or**
- A JSON object with a nested `data.url` or `data.link` key, **or**
- A raw HTTPS URL string in plain text.

Debug with `curl`:

```bash
curl -v -X POST https://screenshots.example.com/upload \
  -F "image=@screenshot.png;type=image/png"
```

### "Upload endpoint is not configured in Preferences"

You haven't entered a URL in **Preferences → Upload & Network → Upload Endpoint**, or the field is blank. Enter your server's URL and click OK.

### Uploads succeed but the link opens a blank page

The `BASE_URL` environment variable on your server doesn't match the public-facing address. For instance, if Nginx terminates TLS on `screenshots.example.com` but the server's `BASE_URL` is still `http://localhost:9100`, the returned URL will point to localhost. Set `BASE_URL` to your actual domain.

### Certificate errors / TLS handshake failures

LinuxShot uses Qt's `QNetworkAccessManager`, which relies on the system's CA certificate store. Ensure:
- Your TLS certificate is from a trusted CA (Let's Encrypt works).
- Self-signed certs require adding the CA to your system's trust store:
  ```bash
  sudo cp myCA.crt /usr/local/share/ca-certificates/
  sudo update-ca-certificates
  ```

### How do I add upload authentication to the LinuxShot client?

Currently, `SelfHostedProvider` does not send custom headers beyond `User-Agent`. To add `Authorization` header support, modify [`SelfHostedProvider.cpp`](file:///home/ameya/linuxshot/src/upload/SelfHostedProvider.cpp#L62-L64) to read an auth token from `SettingsManager` and add it to the request:

```cpp
// After building the QNetworkRequest:
QString authToken = settingsManager->uploadAuthToken();
if (!authToken.isEmpty()) {
    request.setRawHeader("Authorization",
                         QStringLiteral("Bearer %1").arg(authToken).toUtf8());
}
```

This is a straightforward enhancement — contributions welcome!

### Can I use Docker?

Absolutely. Wrap any of the servers above in a `Dockerfile`:

```dockerfile
FROM python:3.12-slim
WORKDIR /app
COPY upload_server.py .
RUN pip install flask
ENV UPLOAD_DIR=/data
VOLUME /data
EXPOSE 9100
CMD ["python3", "upload_server.py"]
```

```bash
docker build -t linuxshot-upload .
docker run -d -p 9100:9100 \
  -e API_KEY="your-secret" \
  -e BASE_URL="https://screenshots.example.com" \
  -v /srv/linuxshot-uploads:/data \
  linuxshot-upload
```

---

*Last updated: 2026-10-31*

