#!/usr/bin/env python3
"""IDA SDK manifest helper for local builds and GitHub Actions."""

from __future__ import annotations

import argparse
import hashlib
import hmac
import json
import os
import platform
import secrets
import shutil
import subprocess
import sys
import urllib.request
import zipfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "ghidradec.targets.json"
PRIVATE_SDK_DIR = ROOT / ".idasdks"
PUBLISHABLE_SDK_DIR = ROOT / "idasdks"
CRYPTO_MAGIC = b"GDECSDK1"
CRYPTO_ITERATIONS = 200000
AES_BLOCK_SIZE = 16
HMAC_SIZE = 32
AES_SBOX = [
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16,
]
AES_INV_SBOX = [
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D,
]
AES_RCON = [0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40]


def load_manifest() -> dict:
    return json.loads(MANIFEST.read_text(encoding="utf-8"))


def host_platform() -> str:
    system = platform.system().lower()
    if system == "darwin":
        return "macos"
    if system.startswith("windows"):
        return "windows"
    if system == "linux":
        return "linux"
    return system


def find_sdk(manifest: dict, version: str) -> dict:
    for sdk in manifest["idaSdks"]:
        if sdk["version"] == version:
            return sdk
    raise SystemExit(f"IDA SDK {version} is not defined in {MANIFEST}")


def extract_archive(archive: Path, destination_root: Path, directory: str) -> Path:
    if not archive.exists():
        raise SystemExit(f"Archive not found: {archive}")

    destination_root.mkdir(parents=True, exist_ok=True)
    sdk_dir = destination_root / directory
    if sdk_dir.exists() and (sdk_dir / "include" / "ida.hpp").exists():
        return sdk_dir

    scratch = destination_root / f".extract-{directory}"
    if scratch.exists():
        shutil.rmtree(scratch)
    scratch.mkdir(parents=True)

    with zipfile.ZipFile(archive) as zf:
        zf.extractall(scratch)

    nested_archive = scratch / f"{directory}.zip"
    if not any(scratch.rglob("include/ida.hpp")) and nested_archive.exists():
        nested_scratch = scratch / ".nested"
        nested_scratch.mkdir(parents=True)
        with zipfile.ZipFile(nested_archive) as zf:
            zf.extractall(nested_scratch)
        for child in list(scratch.iterdir()):
            if child != nested_scratch:
                if child.is_dir():
                    shutil.rmtree(child)
                else:
                    child.unlink()
        for child in nested_scratch.iterdir():
            shutil.move(str(child), str(scratch / child.name))
        shutil.rmtree(nested_scratch, ignore_errors=True)

    source_dir = scratch
    for marker in scratch.rglob("include/ida.hpp"):
        source_dir = marker.parent.parent
        break
    if not (source_dir / "include" / "ida.hpp").exists():
        raise SystemExit(f"{archive} did not extract to a recognizable IDA SDK layout")

    if sdk_dir.exists():
        shutil.rmtree(sdk_dir)
    shutil.move(str(source_dir), str(sdk_dir))
    shutil.rmtree(scratch, ignore_errors=True)
    return sdk_dir


def download(url: str, output: Path) -> Path:
    output.parent.mkdir(parents=True, exist_ok=True)
    if output.exists():
        return output
    print(f"Downloading {url}")
    with urllib.request.urlopen(url) as response, output.open("wb") as handle:
        shutil.copyfileobj(response, handle)
    return output


def password_from_args(args: argparse.Namespace) -> str:
    password = args.password or os.environ.get("IDA_SDK_AES_KEY")
    if not password:
        raise SystemExit("Password was not provided. Pass --password or set IDA_SDK_AES_KEY.")
    return password


def derive_key_material(salt: bytes, password: str) -> bytes:
    return hashlib.pbkdf2_hmac(
        "sha256",
        password.encode("utf-8"),
        salt,
        CRYPTO_ITERATIONS,
        dklen=64,
    )


def aes_xtime(value: int) -> int:
    value <<= 1
    if value & 0x100:
        value ^= 0x11B
    return value & 0xFF


def aes_mul(left: int, right: int) -> int:
    result = 0
    while right:
        if right & 1:
            result ^= left
        left = aes_xtime(left)
        right >>= 1
    return result


def aes_expand_key(key: bytes) -> list[bytes]:
    if len(key) != 32:
        raise ValueError("AES-256 requires a 32-byte key")
    words = [list(key[i:i + 4]) for i in range(0, len(key), 4)]
    for i in range(8, 60):
        temp = words[i - 1][:]
        if i % 8 == 0:
            temp = temp[1:] + temp[:1]
            temp = [AES_SBOX[value] for value in temp]
            temp[0] ^= AES_RCON[(i // 8) - 1]
        elif i % 8 == 4:
            temp = [AES_SBOX[value] for value in temp]
        words.append([words[i - 8][j] ^ temp[j] for j in range(4)])
    return [bytes(sum(words[i:i + 4], [])) for i in range(0, len(words), 4)]


def aes_add_round_key(state: list[int], round_key: bytes) -> None:
    for i in range(16):
        state[i] ^= round_key[i]


def aes_sub_bytes(state: list[int], inverse: bool = False) -> None:
    table = AES_INV_SBOX if inverse else AES_SBOX
    for i in range(16):
        state[i] = table[state[i]]


def aes_shift_rows(state: list[int], inverse: bool = False) -> None:
    shifts = (0, 3, 2, 1) if inverse else (0, 1, 2, 3)
    for row, shift in enumerate(shifts):
        values = [state[row + 4 * col] for col in range(4)]
        values = values[shift:] + values[:shift]
        for col, value in enumerate(values):
            state[row + 4 * col] = value


def aes_mix_columns(state: list[int], inverse: bool = False) -> None:
    matrix = (14, 11, 13, 9) if inverse else (2, 3, 1, 1)
    for col in range(4):
        offset = 4 * col
        a0, a1, a2, a3 = state[offset:offset + 4]
        if inverse:
            state[offset + 0] = aes_mul(a0, matrix[0]) ^ aes_mul(a1, matrix[1]) ^ aes_mul(a2, matrix[2]) ^ aes_mul(a3, matrix[3])
            state[offset + 1] = aes_mul(a0, matrix[3]) ^ aes_mul(a1, matrix[0]) ^ aes_mul(a2, matrix[1]) ^ aes_mul(a3, matrix[2])
            state[offset + 2] = aes_mul(a0, matrix[2]) ^ aes_mul(a1, matrix[3]) ^ aes_mul(a2, matrix[0]) ^ aes_mul(a3, matrix[1])
            state[offset + 3] = aes_mul(a0, matrix[1]) ^ aes_mul(a1, matrix[2]) ^ aes_mul(a2, matrix[3]) ^ aes_mul(a3, matrix[0])
        else:
            state[offset + 0] = aes_mul(a0, matrix[0]) ^ aes_mul(a1, matrix[1]) ^ a2 ^ a3
            state[offset + 1] = a0 ^ aes_mul(a1, matrix[0]) ^ aes_mul(a2, matrix[1]) ^ a3
            state[offset + 2] = a0 ^ a1 ^ aes_mul(a2, matrix[0]) ^ aes_mul(a3, matrix[1])
            state[offset + 3] = aes_mul(a0, matrix[1]) ^ a1 ^ a2 ^ aes_mul(a3, matrix[0])


def aes_encrypt_block(block: bytes, round_keys: list[bytes]) -> bytes:
    state = list(block)
    aes_add_round_key(state, round_keys[0])
    for round_index in range(1, 14):
        aes_sub_bytes(state)
        aes_shift_rows(state)
        aes_mix_columns(state)
        aes_add_round_key(state, round_keys[round_index])
    aes_sub_bytes(state)
    aes_shift_rows(state)
    aes_add_round_key(state, round_keys[14])
    return bytes(state)


def aes_decrypt_block(block: bytes, round_keys: list[bytes]) -> bytes:
    state = list(block)
    aes_add_round_key(state, round_keys[14])
    for round_index in range(13, 0, -1):
        aes_shift_rows(state, inverse=True)
        aes_sub_bytes(state, inverse=True)
        aes_add_round_key(state, round_keys[round_index])
        aes_mix_columns(state, inverse=True)
    aes_shift_rows(state, inverse=True)
    aes_sub_bytes(state, inverse=True)
    aes_add_round_key(state, round_keys[0])
    return bytes(state)


def pkcs7_pad(data: bytes) -> bytes:
    pad_length = AES_BLOCK_SIZE - (len(data) % AES_BLOCK_SIZE)
    return data + bytes([pad_length]) * pad_length


def pkcs7_unpad(data: bytes) -> bytes:
    if not data or len(data) % AES_BLOCK_SIZE != 0:
        raise SystemExit("AES plaintext has invalid PKCS#7 length.")
    pad_length = data[-1]
    if pad_length < 1 or pad_length > AES_BLOCK_SIZE:
        raise SystemExit("AES plaintext has invalid PKCS#7 padding.")
    if data[-pad_length:] != bytes([pad_length]) * pad_length:
        raise SystemExit("AES plaintext has invalid PKCS#7 padding.")
    return data[:-pad_length]


def python_aes_cbc(data: bytes, key: bytes, iv: bytes, decrypt: bool) -> bytes:
    round_keys = aes_expand_key(key)
    if decrypt:
        if len(data) % AES_BLOCK_SIZE != 0:
            raise SystemExit("AES ciphertext length is not block-aligned.")
        previous = iv
        plaintext = bytearray()
        for offset in range(0, len(data), AES_BLOCK_SIZE):
            block = data[offset:offset + AES_BLOCK_SIZE]
            decrypted = aes_decrypt_block(block, round_keys)
            plaintext.extend(left ^ right for left, right in zip(decrypted, previous))
            previous = block
        return pkcs7_unpad(bytes(plaintext))

    previous = iv
    ciphertext = bytearray()
    padded = pkcs7_pad(data)
    for offset in range(0, len(padded), AES_BLOCK_SIZE):
        block = padded[offset:offset + AES_BLOCK_SIZE]
        mixed = bytes(left ^ right for left, right in zip(block, previous))
        encrypted = aes_encrypt_block(mixed, round_keys)
        ciphertext.extend(encrypted)
        previous = encrypted
    return bytes(ciphertext)


def openssl_aes_cbc(data: bytes, key: bytes, iv: bytes, decrypt: bool) -> bytes:
    cmd = [
        "openssl",
        "enc",
        "-aes-256-cbc",
        "-K",
        key.hex(),
        "-iv",
        iv.hex(),
    ]
    if decrypt:
        cmd.append("-d")
    try:
        result = subprocess.run(
            cmd,
            input=data,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
    except FileNotFoundError as exc:
        return python_aes_cbc(data, key, iv, decrypt)
    if result.returncode != 0:
        detail = result.stderr.decode("utf-8", errors="replace").strip()
        raise SystemExit(f"OpenSSL AES operation failed: {detail}")
    return result.stdout


def write_bytes(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)


def encrypt_file(args: argparse.Namespace) -> None:
    password = password_from_args(args)
    input_path = Path(args.input).resolve()
    output_path = Path(args.output).resolve()
    plaintext = input_path.read_bytes()
    salt = secrets.token_bytes(16)
    iv = secrets.token_bytes(AES_BLOCK_SIZE)
    material = derive_key_material(salt, password)
    ciphertext = openssl_aes_cbc(plaintext, material[:32], iv, decrypt=False)
    authenticated = CRYPTO_MAGIC + salt + iv + ciphertext
    tag = hmac.new(material[32:], authenticated, hashlib.sha256).digest()
    write_bytes(output_path, authenticated + tag)


def decrypt_bytes(encrypted: bytes, password: str) -> bytes:
    minimum = len(CRYPTO_MAGIC) + 16 + AES_BLOCK_SIZE + HMAC_SIZE
    if len(encrypted) < minimum:
        raise SystemExit("Encrypted SDK file is too short.")
    if encrypted[: len(CRYPTO_MAGIC)] != CRYPTO_MAGIC:
        raise SystemExit("Encrypted SDK file has an unknown format.")

    salt_start = len(CRYPTO_MAGIC)
    iv_start = salt_start + 16
    cipher_start = iv_start + AES_BLOCK_SIZE
    tag_start = len(encrypted) - HMAC_SIZE
    salt = encrypted[salt_start:iv_start]
    iv = encrypted[iv_start:cipher_start]
    ciphertext = encrypted[cipher_start:tag_start]
    tag = encrypted[tag_start:]

    material = derive_key_material(salt, password)
    expected = hmac.new(material[32:], encrypted[:tag_start], hashlib.sha256).digest()
    if not hmac.compare_digest(tag, expected):
        raise SystemExit("Encrypted SDK authentication failed.")
    return openssl_aes_cbc(ciphertext, material[:32], iv, decrypt=True)


def decrypt_file(args: argparse.Namespace) -> None:
    password = password_from_args(args)
    input_path = Path(args.input).resolve()
    output_path = Path(args.output).resolve()
    write_bytes(output_path, decrypt_bytes(input_path.read_bytes(), password))


def decrypt_private_sdks(args: argparse.Namespace) -> None:
    password = password_from_args(args)
    source = Path(args.source).resolve()
    destination = Path(args.destination).resolve()
    encrypted_archives = sorted(source.glob("*.enc"))
    if not encrypted_archives:
        raise SystemExit(f"No encrypted SDK archives found in {source}")
    destination.mkdir(parents=True, exist_ok=True)
    for encrypted_path in encrypted_archives:
        output_name = encrypted_path.name[:-4]
        output_path = destination / output_name
        write_bytes(output_path, decrypt_bytes(encrypted_path.read_bytes(), password))
        print(output_path)


def ensure_sdk(args: argparse.Namespace) -> None:
    manifest = load_manifest()
    sdk = find_sdk(manifest, args.version or manifest["project"]["defaultIdaSdk"])
    destination_root = Path(args.destination).resolve() if args.destination else PRIVATE_SDK_DIR

    if sdk["source"] in {"public-release", "public-branch"}:
        archive = download(sdk["url"], destination_root / sdk["archive"])
    else:
        archive = destination_root / sdk["archive"]
        if not archive.exists():
            encrypted = PUBLISHABLE_SDK_DIR / f"{sdk['archive']}.enc"
            raise SystemExit(
                f"Private SDK archive missing: {archive}. "
                f"Decrypt {encrypted} first or place {sdk['archive']} in {destination_root}."
            )

    sdk_dir = extract_archive(archive, destination_root, sdk["directory"])
    print(sdk_dir)


def matrix(args: argparse.Namespace) -> None:
    manifest = load_manifest()
    include_private = args.include_private or os.environ.get("GHIDRADEC_INCLUDE_PRIVATE_SDKS") == "1"
    only_host = args.current_host
    current = host_platform()
    default_version = manifest["project"]["defaultIdaSdk"]
    requested_versions = {
        version.strip()
        for version in (args.versions or "").split(",")
        if version.strip()
    }
    if args.all and requested_versions:
        raise SystemExit("--all and --versions are mutually exclusive")
    known_versions = {sdk["version"] for sdk in manifest["idaSdks"]}
    unknown_versions = sorted(requested_versions - known_versions)
    if unknown_versions:
        raise SystemExit(
            "Unknown IDA SDK version(s): "
            + ", ".join(unknown_versions)
            + ". Check ghidradec.targets.json for valid version names."
        )
    include = []

    for sdk in manifest["idaSdks"]:
        if not sdk.get("enabled", False):
            continue
        if requested_versions:
            if sdk["version"] not in requested_versions:
                continue
        elif not args.all:
            if sdk["version"] != default_version:
                continue
        if sdk["source"] not in {"public-release", "public-branch"} and not include_private:
            continue
        for os_name in sdk.get("platforms", ["windows", "linux", "macos"]):
            if only_host and os_name != current:
                continue
            runner = {
                "windows": "windows-latest",
                "linux": "ubuntu-latest",
                "macos": "macos-latest",
            }[os_name]
            include.append(
                {
                    "ida": sdk["version"],
                    "os": os_name,
                    "runner": runner,
                    "sdk_dir": sdk["directory"],
                    "sdk_source": sdk["source"],
                    "cmake_var": sdk["cmakeVariable"],
                }
            )

    print(json.dumps({"include": include}, separators=(",", ":")))


def main() -> int:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="command", required=True)

    ensure = sub.add_parser("ensure", help="Download/extract a target IDA SDK")
    ensure.add_argument("--version")
    ensure.add_argument("--destination")
    ensure.set_defaults(func=ensure_sdk)

    enc = sub.add_parser("encrypt", help="Encrypt a private IDA SDK archive")
    enc.add_argument("--input", required=True)
    enc.add_argument("--output", required=True)
    enc.add_argument("--password")
    enc.set_defaults(func=encrypt_file)

    dec = sub.add_parser("decrypt", help="Decrypt a private IDA SDK archive")
    dec.add_argument("--input", required=True)
    dec.add_argument("--output", required=True)
    dec.add_argument("--password")
    dec.set_defaults(func=decrypt_file)

    dec_private = sub.add_parser("decrypt-private", help="Decrypt all committed private SDK archives")
    dec_private.add_argument("--source", default=str(PUBLISHABLE_SDK_DIR))
    dec_private.add_argument("--destination", default=str(PRIVATE_SDK_DIR))
    dec_private.add_argument("--password")
    dec_private.set_defaults(func=decrypt_private_sdks)

    mat = sub.add_parser("matrix", help="Emit GitHub Actions matrix JSON")
    mat.add_argument("--include-private", action="store_true")
    mat.add_argument("--current-host", action="store_true")
    mat.add_argument("--all", action="store_true", help="Include every enabled SDK target.")
    mat.add_argument("--versions", help="Comma-separated SDK versions to include.")
    mat.set_defaults(func=matrix)

    args = parser.parse_args()
    args.func(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
