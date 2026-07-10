#!/usr/bin/env python3
"""Serve private IDA SDK archives to a short-lived release workflow.

This intentionally uses only the Python standard library. Put the licensed SDK
archives in a directory, run this server bound to localhost, and expose it with
a temporary HTTPS tunnel such as cloudflared, ngrok, or Tailscale Funnel.
"""

from __future__ import annotations

import argparse
import http.server
import secrets
import ssl
import sys
import urllib.parse
from pathlib import Path


class PrivateSdkHandler(http.server.BaseHTTPRequestHandler):
    root: Path
    token: str

    server_version = "GhidraDecPrivateSdk/1.0"

    def do_HEAD(self) -> None:
        self.serve_archive(send_body=False)

    def do_GET(self) -> None:
        self.serve_archive(send_body=True)

    def serve_archive(self, *, send_body: bool) -> None:
        archive = self.resolve_archive()
        if archive is None:
            return

        try:
            size = archive.stat().st_size
            self.send_response(200)
            self.send_header("Content-Type", "application/zip")
            self.send_header("Content-Length", str(size))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            if send_body:
                with archive.open("rb") as handle:
                    while chunk := handle.read(1024 * 1024):
                        self.wfile.write(chunk)
        except OSError:
            self.send_error(404, "Archive not found")

    def resolve_archive(self) -> Path | None:
        url = urllib.parse.urlsplit(self.path)
        parts = [urllib.parse.unquote(part) for part in url.path.split("/") if part]

        authorized = not self.token
        if self.token and parts and secrets.compare_digest(parts[0], self.token):
            authorized = True
            parts = parts[1:]

        if self.token and not authorized:
            auth = self.headers.get("Authorization", "")
            if auth.startswith("Bearer "):
                authorized = secrets.compare_digest(auth.removeprefix("Bearer ").strip(), self.token)
            header_token = self.headers.get("X-GhidraDec-Token", "")
            if header_token:
                authorized = authorized or secrets.compare_digest(header_token.strip(), self.token)

        if not authorized:
            self.send_error(401, "Unauthorized")
            return None

        if len(parts) != 1 or parts[0] in {".", ".."} or "/" in parts[0] or "\\" in parts[0]:
            self.send_error(404, "Archive not found")
            return None

        archive = (self.root / parts[0]).resolve()
        try:
            archive.relative_to(self.root)
        except ValueError:
            self.send_error(404, "Archive not found")
            return None

        if not archive.is_file():
            self.send_error(404, "Archive not found")
            return None

        return archive

    def log_message(self, fmt: str, *args: object) -> None:
        url = urllib.parse.urlsplit(self.path)
        archive = Path(urllib.parse.unquote(url.path)).name
        sys.stderr.write("%s - %s\n" % (self.address_string(), archive or "/"))


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Short-lived private IDA SDK archive server.")
    parser.add_argument("--root", default=".idasdks", help="Directory containing SDK archive ZIPs.")
    parser.add_argument("--host", default="127.0.0.1", help="Bind address. Keep localhost when using a tunnel.")
    parser.add_argument("--port", type=int, default=8765)
    parser.add_argument("--token", default="", help="Optional token accepted as /<token>/<archive>.zip or bearer auth.")
    parser.add_argument("--token-file", default="", help="Read the token from a file.")
    parser.add_argument("--cert", default="", help="Optional TLS certificate. Usually the tunnel terminates HTTPS instead.")
    parser.add_argument("--key", default="", help="Optional TLS private key used with --cert.")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    root = Path(args.root).resolve()
    if not root.is_dir():
        raise SystemExit(f"SDK archive root does not exist: {root}")

    token = args.token
    if args.token_file:
        token = Path(args.token_file).read_text(encoding="utf-8").strip()

    handler = type(
        "ConfiguredPrivateSdkHandler",
        (PrivateSdkHandler,),
        {"root": root, "token": token},
    )
    server = http.server.ThreadingHTTPServer((args.host, args.port), handler)

    scheme = "http"
    if args.cert:
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        context.load_cert_chain(args.cert, args.key or None)
        server.socket = context.wrap_socket(server.socket, server_side=True)
        scheme = "https"

    token_path = "/<token>" if token else ""
    print(f"Serving {root} at {scheme}://{args.host}:{args.port}{token_path}/<archive>.zip")
    print("Keep this process running only while the release workflow downloads SDKs.")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nStopped.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
